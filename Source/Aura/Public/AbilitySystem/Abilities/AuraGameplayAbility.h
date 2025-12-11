// Fill out your copyright notice in the Description page of Project Settings.

/* 继承自UGameplayAbility，作为所有Aura GA的基类
 * 需要在蓝图中设置启动技能的标签（如：发射火球设置为鼠标左键标签InputTag.LMB）
 */
#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	// 游戏刚开始就赋予的启动技能的标签
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    FGameplayTag StartupInputTag;
};
