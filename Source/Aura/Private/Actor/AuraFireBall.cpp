// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraFireBall.h"

#include "GameplayCueManager.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"

void AAuraFireBall::BeginPlay()
{
	Super::BeginPlay();
	// 启用Timeline，火球先飞走再飞回
	StartOutgoingTimeline();
}

void AAuraFireBall::OnSphereOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	// 在服务器端
	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			// 填充目标ASC
			DamageEffectParams.DeathImpulse = DeathImpulse;
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			// 应用GE
			UAuraAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void AAuraFireBall::OnHit()
{
	// GameplayCue
	if (GetOwner())
	{
		FGameplayCueParameters CueParams;
		CueParams.Location = GetActorLocation();
		// NonReplicated因为火球的位置和状态已经是网络同步的，各个客户端看到火球命中时，自己在本地播放一次爆炸特效即可，不需要服务器再发一次 RPC 广播
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FAuraGameplayTags::Get().GameplayCue_FireBlast, CueParams);
	}
	
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}
