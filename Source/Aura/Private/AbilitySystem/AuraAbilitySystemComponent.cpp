// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 当 GE 应用到自身时，注册回调函数
	// 即只要自己身上有新的 GameplayEffect 应用，就会自动调用 EffectApplied(...)
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::EffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// FGameplayAbilitySpec是玩家身上拥有的该能力的具体实例数据
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass);
		// 将能力添加到角色的ASC
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// 从EffectSpec中获取所有Tag
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	// 广播标签，在OverlayWidgetController的Lambda会收到通知，执行UI逻辑
	EffectAssetTags.Broadcast(TagContainer);
}
