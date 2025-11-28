// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemBlueprintLibrary.h"


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot")));
}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectToEnemy) return;
	
	// 从Target获取AbilitySystemComponent
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;	// 没有 ASC 的对象没法接受 GE

	check(GameplayEffectClass);
	// EffectContext用来描述这次效果应用的上下文（来源是谁、是否是暴击、命中信息等）
	// MakeEffectContext()创建一个空的EffectContext
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	// AddSourceObject(this) 把 AAuraEffectActor（也就是 GE 的施加者）作为来源写进 Context，方便以后在 GE 或者 GameplayCue 里追踪。
	EffectContextHandle.AddSourceObject(this);

	// GameplayEffectSpec是GE的实例包含了等级、上下文、Modifiers 等
	// MakeOutgoingSpec用来创建一个实例，参数：GE类，等级，EffectContext
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	// 把GE应用到TargetASC上，注意参数为const FGameplayEffectSpec&
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	// 判断是否为无限持续的效果
	// .Data是一个TSharedPtr<FGameplayEffectSpec>  .Get()获得原始指针FGameplayEffectSpec*
	// ->Def FGameplayEffectSpec内部有个 Def 成员，指向TWeakObjectPtr<const UGameplayEffect>
	// .Get()获取WeakObjectPtr原始指针UGameplayEffect*
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	// 是无限持续的效果 且 需要移除  才需要保存句柄到ASC的映射
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	// 如果不是无限持续的效果 且 应当被摧毁 则销毁
	if (!bIsInfinite && bDestroyOnEffectApplication)
	{
		Destroy();
	}
	
}

void AAuraEffectActor::OnOverlap(AActor* TargetActor)
{
	// 避免某些效果应用到敌人
	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectToEnemy) return;
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}

}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag("Enemy") && !bApplyEffectToEnemy) return;
	
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	// 移除Infinite的效果
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;

		// 待移除的GE，不要在循环中删除，先将其加入数组
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (auto& HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				// 1表示移除一层，默认-1表示全部移除
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}
