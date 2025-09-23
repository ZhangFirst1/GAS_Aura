// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Interaction/EnemyInterface.h"


AAuraPlayerController::AAuraPlayerController()
{
	// 该控制器对象会在网络环境复制，在客户端和服务器保持一致
	bReplicates = true;
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
}

void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;

	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

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
	if (LastActor == nullptr)
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
	}
	
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// 将控制器自带的输入组件InputComponent转为增强输入的组件
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// 绑定
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
}


void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();							// 获取控制器的朝向（由相机决定）
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.0f);		// 只获取Yaw，保证移动在水平面上

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



