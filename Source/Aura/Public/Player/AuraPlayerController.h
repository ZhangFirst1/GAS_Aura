// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class USplineComponent;
struct FGameplayTag;
class UAuraInputConfig;
class UInputMappingContext;
class UInputAction;
class IEnemyInterface;
struct FInputActionValue;
class UAuraAbilitySystemComponent;
class UDamageTextComponent;

/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController(); 
	virtual void PlayerTick(float DeltaTime) override;

	// 客户端RPC，显示伤害数字，在后处理中调用
	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit);
protected:
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

private:
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> AuraContext;
	
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> ShiftAction;
	
	void ShiftPressed() { bShiftKeyDown = true; };
	void ShiftReleased() { bShiftKeyDown = false; };
	bool bShiftKeyDown = false;
	
	void Move(const FInputActionValue& InputActionValue);

	// 用于区分鼠标选中的对象
	void CursorTrace();
	IEnemyInterface* LastActor;
	IEnemyInterface* ThisActor;
	FHitResult CursorHit;

	// 按下 松开 按住 的动作
	void AbilityInputTagPressed(const FGameplayTag InputTag);
	void AbilityInputTagReleased(const FGameplayTag InputTag);
	void AbilityInputTagHeld(const FGameplayTag InputTag);

	// UAuraInputConfig组合了 InputAction 和 Tag
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	TObjectPtr<UAuraInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	// 获取ASC，防止在Held中每帧都调用转换
	UAuraAbilitySystemComponent* GetASC();

	// 用于鼠标移动
	FVector CachedDestination = FVector::ZeroVector;	// 目的地
	float FollowTime = 0.f;								// 移动时间
	float ShortPressThreshold = 1.f;					// 多久算短按
	bool bAutoRunning = false;							// 是否自动移动
	bool bTargeting = false;							// 是否选中目标

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f;				// 到目的地内的半径

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<USplineComponent> Spline;				// 移动曲线

	void AutoRun();
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;	// 显示伤害文本Component
};
