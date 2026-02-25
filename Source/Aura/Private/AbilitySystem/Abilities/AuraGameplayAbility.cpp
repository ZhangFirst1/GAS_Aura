// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

#include "AbilitySystem/AuraAttributeSet.h"

FString UAuraGameplayAbility::GetDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>%s, </><Level>%d</>"), L"Default Ability", Level);
}

FString UAuraGameplayAbility::GetNextLevelDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>NextLevel: </><Level>%d</> \n <Default>Causes much more damage.</>"), Level);
}

FString UAuraGameplayAbility::GetLockedDescription(int32 Level)
{
	return FString::Printf(TEXT("<Default>Spell Locked Until Level: %d</>"), Level);
}

float UAuraGameplayAbility::GetManaCost(float InLevel) const
{
	float ManaCost = 0.f;
	// GetCostGameplayEffect()会返回你在蓝图中配置的 "Cost Gameplay Effect Class" 的默认对象
	if (const UGameplayEffect* CostEffect = GetCostGameplayEffect())
	{
		// CostEffect->Modifiers代表了GE可以修改的全部属性
		for (FGameplayModifierInfo Mod: CostEffect->Modifiers)
		{
			// 如果修改的是Mana
			if (Mod.Attribute == UAuraAttributeSet::GetManaAttribute())
			{
				// 根据等级查询ScalableFloat或CurveTable对应的值，并存在ManaCost中
				Mod.ModifierMagnitude.GetStaticMagnitudeIfPossible(InLevel, ManaCost);
				break;
			}
		}
	}
	return ManaCost;
}

float UAuraGameplayAbility::GetCooldown(float InLevel) const
{
	float Cooldown = 0.f;
	if (const UGameplayEffect* CooldownEffect = GetCooldownGameplayEffect())
	{
		CooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(InLevel, Cooldown);
	}
	return Cooldown;
}
