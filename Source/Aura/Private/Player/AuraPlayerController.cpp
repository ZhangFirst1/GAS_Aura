// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "GameplayTagContainer.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"


AAuraPlayerController::AAuraPlayerController()
{
	// 该控制器对象会在网络环境复制，在客户端和服务器保持一致
	bReplicates = true;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(AuraContext);

	// 获取本地玩家的增强输入子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if(Subsystem)
	{
		// 绑定输入映射上下文
		Subsystem->AddMappingContext(AuraContext, 0);
	}

	// 显示鼠标光标，设置为默认光标样式（箭头）
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;

	// Input Mode(输入模式)决定玩家如何与UI和游戏交互
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);	// 鼠标不会被锁定在窗口内
	InputModeData.SetHideCursorDuringCapture(false);							// 点击窗口后，光标仍然可见
	SetInputMode(InputModeData);												
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();

	AutoRun();
}

void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		// 这里使用NewObject动态创建，还需注册Component，等同于CreateDefaultSubobject
		UDamageTextComponent* DamageTextComponent = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageTextComponent->RegisterComponent();
		// 让伤害数字出现在角色头顶，但不再随角色移动
		DamageTextComponent->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageTextComponent->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageTextComponent->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}

void AAuraPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	if (APawn* ControlledPawn = GetPawn())
	{
		// 角色在路径上的最近位置
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// 该路径点的切线方向向量
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		// 距离小于AutoRunAcceptanceRadius则停止
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void AAuraPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
	
	/**
	 *	Line Trace from cursor. There are several scenarios:
	 *	A. LastActor is null && ThisActor is null
	 *		- Do nothing.
	 *	B. LastActor is null && ThisActor is valid
	 *		- Highlight ThisActor.
	 *	C. LastActor is valid && ThisActor is null
	 *		- UnHighlight LastActor
	 *	D. Both Actor are valid. But LastActor != ThisActor
	 *		- UnHighlight LastActor, and Highlight ThisActor.
	 *	E. Both Actor are valid. But LastActor == ThisActor
	 *		- Do nothing.
	 * **/
	/*if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// B
			ThisActor->HighlightActor();
		}else
		{
			// A 
		}
	}else
	{
		if (ThisActor == nullptr)
		{
			// C
			LastActor->UnHighlightActor();
		}else
		{
			if (LastActor != ThisActor)
			{
				// D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}else
			{
				// E
				
			}
		}
	}*/
}

void AAuraPlayerController::AbilityInputTagPressed(const FGameplayTag InputTag)
{
	 // 点击鼠标左键
	if (InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		// 根据鼠标目标，赋值是否选中目标
		bTargeting = ThisActor ? true : false;
		bAutoRunning = false;		// 在松开鼠标前还不确定是否为短按鼠标
	}
}

void AAuraPlayerController::AbilityInputTagReleased(const FGameplayTag InputTag)
{
	// 没按鼠标左键
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}

	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

	// 没选中Target且没按住shift时，才会检测松开鼠标左键移动
	if (!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		// 松开按键时 是否为短按左键，若是则开始自动寻路
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				// 每次导航前清空所有点
				Spline->ClearSplinePoints();
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				// 将目的地设置为导航的最后一个点，而非鼠标点击点，防止点击物体内部造成的无法到达
				if (NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints.Last();
					bAutoRunning = true;
				}
			}
		}
		// 重置
		FollowTime = 0.f;
		bTargeting = false;
	}
	
}

void AAuraPlayerController::AbilityInputTagHeld(const FGameplayTag InputTag)
{
	// 没按鼠标左键
	if (!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	// 按鼠标左键，且  有选中目标或按住shift
	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);	
	}
	else    // 按左键，未选中目标，需要跟随鼠标移动
	{
		FollowTime += GetWorld()->GetDeltaSeconds();

		// 获取碰撞
		if (CursorHit.bBlockingHit)	CachedDestination = CursorHit.ImpactPoint;
		
		// 使用AddMovementInput移动
		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}

UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if (AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent = Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 将控制器自带的输入组件InputComponent转为 自定义的增强输入的组件
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	// 绑定
	AuraInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);

	// 批量绑定，为每个InputAction绑定 按下、松开、和按住对应的回调
	AuraInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();									// 获取控制器的朝向（由相机决定）
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.0f);		        // 只获取Yaw，保证移动在水平面上

	// 根据YawRotation生成旋转矩阵，前后左右会跟随相机朝向
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// X表示左右，Y表示前后
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}



