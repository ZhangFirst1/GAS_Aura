// Fill out your copyright notice in the Description page of Project Settings.

/* 继承自UAbilitySystemComponent，实现自定义的ASC
 * 1. 管理角色输入
 * 2. 管理初始能力
 * 3. 接收 GE 应用时的回调，广播通知 UI
 */

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilitySystemComponent.generated.h"

// 多播委托，当被应用 GameplayEffect 时，将其 Tags 广播出去
DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /* Asset Tags */);
DECLARE_MULTICAST_DELEGATE(FAbilityGiven);
DECLARE_DELEGATE_OneParam(FForEachAbility, const FGameplayAbilitySpec&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FAbilityStatusChanged, const FGameplayTag& /* AbilityTag */, const FGameplayTag& /* StatusTag */);

/**
 * 
 */
UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UAuraAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
public:
	// 构造函数，注册回调
	void AbilityActorInfoSet();

	// 委托
	FEffectAssetTags EffectAssetTags;
	FAbilityGiven AbilitiesGivenDelegate;
	FAbilityStatusChanged AbilityStatusChanged;

	// 为角色添加出生就有的技能
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;

	// 根据Tag查找能力，并激活 / 释放
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	void ForEachAbility(const FForEachAbility& Delegate);

	// 根据AbilitySpec获取它所属的Tag
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	// 根据Tag获取FGameplayAbilitySpec
	FGameplayAbilitySpec* GetSpecFromAbilityTag(const FGameplayTag& AbilityTag);
	
	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);

	void UpdateAbilityStatus(int32 Level);
protected:
	// 在客户端复制ActivateAbilities
	virtual void OnRep_ActivateAbilities() override;

	
	// 应用Effect，获取并广播所有Tag
	// Client表示客户端RPC，在服务器上执行，下发到客户端，Reliable表示可靠传输
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);

	UFUNCTION(Client, Reliable)
	void ClientUpdateAbilityStatus(const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag);
};