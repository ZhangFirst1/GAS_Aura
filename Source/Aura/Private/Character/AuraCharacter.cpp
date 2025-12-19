// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HUD/AuraHUD.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"

AAuraCharacter::AAuraCharacter()
{
	// 角色会面向移动的方向
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0, 450.f, 0);	// 转向速度
	GetCharacterMovement()->bConstrainToPlane = true;		// 限制角色运动在一个平面内
	GetCharacterMovement()->bSnapToPlaneAtStart = true;		// 游戏开始时，将角色位置对齐到这个平面上

	// 控制器不驱动角色旋转，角色不会因为相机旋转而立刻转身
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	CharacterClass = ECharacterClass::Elementalist;
}

void AAuraCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// 为服务器初始化Ability Actor
	InitAbilityActorInfo();

	// 添加能力
	AddCharacterAbilities();
}

void AAuraCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	// 为客户端初始化Ability Actor
	InitAbilityActorInfo();
}

void AAuraCharacter::AddToXP_Implementation(int32 InXP)
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	AuraPlayerState->AddXP(InXP);
}

void AAuraCharacter::LevelUp_Implementation()
{
	
}

int32 AAuraCharacter::GetPlayLevel() const
{
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	return AuraPlayerState->GetPlayLevel();
}

void AAuraCharacter::InitAbilityActorInfo()
{
	// 获取PlayerState
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	check(AuraPlayerState);
	// 初始化AbilitySystemComponent，指定Owner和Avatar
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);
	// 调用ASC的初始化设置
	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	AttributeSet = AuraPlayerState->GetAttributeSet();

	// 为什么此处用if而非check？
	// 在多人游戏中，客户端只有自己的PlayerController是有效的，其他为空
	// 这种情况AuraPlayerController为空是被允许的，但若使用check会导致崩溃
	if (AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController())){
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			// 初始化HUD
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitiallizeDefaultAbilities();
}
