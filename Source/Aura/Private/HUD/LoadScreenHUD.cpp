// Fill out your copyright notice in the Description page of Project Settings.


#include "HUD/LoadScreenHUD.h"

#include "Blueprint/UserWidget.h"
#include "UI/ViewModel/MVVM_LoadScreen.h"
#include "UI/Widget/LoadScreenWidget.h"

void ALoadScreenHUD::BeginPlay()
{
	Super::BeginPlay();

	// 创建ViewModel
	LoadScreenViewModel = NewObject<UMVVM_LoadScreen>(this, LoadScreenViewModeClass);
	LoadScreenViewModel->InitializeLoadSlots();

	// 创建Widget
	LoadScreenWidget = CreateWidget<ULoadScreenWidget>(GetWorld(), LoadScreenWidgetClass);
	LoadScreenWidget->AddToViewport();
	LoadScreenWidget->BlueprintInitializeWidget();

	LoadScreenViewModel->LoadData();
}
