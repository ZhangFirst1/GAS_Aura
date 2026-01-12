// Fill out your copyright notice in the Description page of Project Settings.



#include "AbilitySystem/MMC/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	// 设置要捕获的属性为 Vigor
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	// 指定从 Target（效果的接受者） 身上捕获这个属性
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	// 非快照会在每次计算时动态读取最新数值
	VigorDef.bSnapshot = false;

	// 把 VigorDef 加入到 需要捕获的属性列表，这样MMC在计算时会自动去抓取这个值
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// 获取 Source 和 Target 的标签
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// 从目标身上获取 Vigor 的当前数值
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluationParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	// 获取Source的等级
	int32 PlayerLevel = 1;
	if (Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayLevel(Spec.GetContext().GetSourceObject());
	}

	return 80.f + 2.5f * Vigor + 10.f * PlayerLevel;
}