// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	// 当 GE 应用到自身时，注册回调函数
	// 即只要自己身上有新的 GameplayEffect 应用，就会自动调用 EffectApplied(...)
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAuraAbilitySystemComponent::ClientEffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		// FGameplayAbilitySpec是玩家身上拥有的该能力的具体实例数据
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);

		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			// 为能力添加一个与之对应的标签
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);
			// 将能力添加到角色的ASC
			GiveAbility(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	// 遍历所有可激活的能力
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 该能力与输入标签对应
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 标记输入状态为 Pressed
			AbilitySpecInputPressed(AbilitySpec);
			// 如果未激活，尝试激活能力
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	// 遍历所有可激活的能力
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		// 该能力与输入标签对应
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			// 标记输入状态为 Released
			AbilitySpecInputReleased(AbilitySpec);
		}
	}
}

void UAuraAbilitySystemComponent::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	// 从EffectSpec中获取所有Tag
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);

	// 广播标签，在OverlayWidgetController的Lambda会收到通知，执行UI逻辑
	EffectAssetTags.Broadcast(TagContainer);
}
