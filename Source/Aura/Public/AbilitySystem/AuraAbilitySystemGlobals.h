// Fill out your copyright notice in the Description page of Project Settings.

/* 重写自定义的AbilitySystemGlobals
 * 所有 GameplayEffect 使用自定义的 Effect Context
 */

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
