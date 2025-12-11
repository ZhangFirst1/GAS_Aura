// Fill out your copyright notice in the Description page of Project Settings.

/* 单例的资源管理器AuraAssetManager，用于加载资源
 * 该初始化函数执行很早，主要用于加载Tag
 */

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "AuraAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UAuraAssetManager& Get();
protected:
	virtual void StartInitialLoading() override;
};
