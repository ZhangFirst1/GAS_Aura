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
	FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, 1.f, EffectContextHandle);
	// 把GE应用到TargetASC上，注意参数为const FGameplayEffectSpec&
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());
}
