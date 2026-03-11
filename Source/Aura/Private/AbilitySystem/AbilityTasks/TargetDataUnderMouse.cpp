// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"
#include "Aura/Aura.h"

/*
*[客户端]
 ├─ Activate()
 │   ├─ IsLocallyControlled() → true
 │   ├─ SendMouseCursorData()
 │   │   ├─ 光标射线检测 → HitResult
 │   │   ├─ 构造 TargetDataHandle
 │   │   ├─ 调用 ServerSetReplicatedTargetData() 发送
 │   │   └─ Broadcast(ValidData) 本地立即生效
 └─────────────────────────────────────────►
[服务器]
	 ├─ Activate()
	 │   ├─ IsLocallyControlled() → false
	 │   ├─ 注册委托等待客户端数据
	 │   ├─ 若客户端已发 → CallReplicatedTargetDataDelegatesIfSet()
	 │   └─ 否则 → SetWaitingOnRemotePlayerData()
	 │
	 ├─ （稍后客户端数据到达）
	 │   ├─ OnTargetDataReplicatedCallback(Data)
	 │   ├─ 广播 ValidData
	 │   └─ EndTask()

 * 
 */

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	// 判断是否为本地控制
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		// 在客户端
		SendMouseCursorData();
	}else
	{
		// 在服务器端
		// 获取技能实例句柄和预测键
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey PredictionKey = GetActivationPredictionKey();
		// 注册委托回调，当服务器拿到DataHandle时，会自动调用
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, PredictionKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);
		// 如果客户端在注册委托前就已经发送完 TargetData，那么这里会立即调用委托函数，不需要等待。
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, PredictionKey);
		// 还没有数据，等待客户端传送
		if (!bCalledDelegate)
		{
			// 会让任务保持激活状态，等待客户端数据到达后再触发回调
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// 开启预测窗口，它确保发送的目标数据（TargetData）在服务器确认之前不会被丢弃或无效
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	// 获取鼠标位置
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Target, false, CursorHit);

	// GAS所有目标信息需要包装在DataHandle中
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	// 发送到服务器
	// GetAbilitySpecHandle()当前技能实例的唯一标识；GetActivationPredictionKey()当前激活的预测键，用于客户端与服务器匹配；DataHandle数据；FGameplayTag()附带一个标记，用于区分不同目标请求
	// AbilitySystemComponent->ScopedPredictionKey当前的预测作用域键，用于客户端预测标识
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(), GetActivationPredictionKey(), DataHandle, FGameplayTag(), AbilitySystemComponent->ScopedPredictionKey);

	// 广播给本地监听者（蓝图）
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,
	FGameplayTag ActivationTag)
{
	// 表示已经处理完这条目标数据，将它从缓存中移除
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
