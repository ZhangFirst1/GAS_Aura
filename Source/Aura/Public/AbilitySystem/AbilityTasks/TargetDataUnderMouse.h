// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);

/**
 * 
 */
UCLASS()
class AURA_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()
public:
	// HidePin: 在蓝图中隐藏形参 ; DefaultToSelf：OwningAbility在蓝图中自定赋值为self ; BlueprintInternalUseOnly：仅用于 Task Event 内部使用
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse", HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	// 节点的输出节点是委托
	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData;
private:
	// Activate会在节点执行时自动调用，根据当前是服务器或客户端，接收或发送数据
	virtual void Activate() override;

	// 发送鼠标位置到服务器并本地广播
	void SendMouseCursorData();

	// 服务器接收到数据的回调函数
	void OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
};
