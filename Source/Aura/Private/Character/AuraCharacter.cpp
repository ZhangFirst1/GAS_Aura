// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacter.h"

#include "GameFramework/CharacterMovementComponent.h"

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
}
