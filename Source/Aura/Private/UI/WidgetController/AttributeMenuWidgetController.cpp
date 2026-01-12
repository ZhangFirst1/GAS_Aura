// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "Player/AuraPlayerState.h"

// 每次打开菜单都会重新初始化值
void UAttributeMenuWidgetController::BroadcastInitialValues()
{
    UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
    check(AttributeInfo);

    // 变量Map，执行Value的函数获取AttributeValue，并将其广播
    for (auto& Pair : AS->TagsToAttributes)
    {
        BroadcastAttributeInfo(Pair.Key, Pair.Value());
    }

    AttribtuePointsChangedDelegate.Broadcast(GetAuraPS()->GetAttributePoints());
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
    check(AttributeInfo);

    // 当属性变化时广播
    for (auto& Pair : GetAuraAS()->TagsToAttributes)
    {
        AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
            [this, Pair](const FOnAttributeChangeData& Data)
            {
                BroadcastAttributeInfo(Pair.Key, Pair.Value());
            }
        );
    }
    
    GetAuraPS()->OnAttributePointsChangedDelegate.AddLambda(
        [this](int32 Points)
        {
            AttribtuePointsChangedDelegate.Broadcast(Points);
        });
    
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
    UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent);
    AuraASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
                                                            const FGameplayAttribute& Attribute) const
{
    FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
    // 根据FGameplayAttribute获取数值（需要其属于的UAttributeSet）
    Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
    // 广播Info
    AttributeInfoDelegate.Broadcast(Info);
}
