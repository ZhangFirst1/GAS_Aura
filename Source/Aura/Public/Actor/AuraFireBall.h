// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actor/AuraProjectile.h"
#include "AuraFireBall.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraFireBall : public AAuraProjectile
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintImplementableEvent)
	void StartOutgoingTimeline();

	// 火球飞到最远处要返回的Actor
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<AActor> ReturnToActor;

	// 火球爆炸时的范围伤害参数
	UPROPERTY(BlueprintReadWrite)
	FDamageEffectParams ExplosionDamageParams;
protected:
	virtual void BeginPlay() override;
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlapComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	// 重写处理火球爆炸逻辑
	virtual void OnHit() override;
};
