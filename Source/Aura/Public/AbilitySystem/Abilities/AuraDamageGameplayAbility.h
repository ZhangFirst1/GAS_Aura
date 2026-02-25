// Fill out your copyright notice in the Description page of Project Settings.

/* 可造成多种伤害类型（通过 SetByCaller）的 Gameplay Ability
 * 作为伤害技能的基类
 */
#pragma once

#include "CoreMinimal.h"
#include "AuraAbilityTypes.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"

struct FTaggedMontage;
/**
 * 
 */
UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	// 造成伤害
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);

	// 创建 DamageEffectParams
	UFUNCTION(BlueprintPure)
	FDamageEffectParams MakeDamageEffectParamsFromClassDefaults(AActor* TargetActor = nullptr) const;
protected:
	// 施加的伤害效果
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 伤害类型和数值
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	FGameplayTag DamageType;

	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	FScalableFloat Damage;

	// Debuff 信息
	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	float DebuffChance = 20.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	float DebuffDamage = 5.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	float DebuffFrequency = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	float DebuffDuration = 5.f;

	// 死亡冲击
	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	float DeathImpulseMagnitude = 1000.f;

	// 击退
	UPROPERTY(EditDefaultsOnly, Category = "Damage") 
	float KnockbackForceMagnitude = 1000.f;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	float KnockbackChance = 0.f;

	// 从蒙太奇数组中随机返回一个
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages) const;
};
