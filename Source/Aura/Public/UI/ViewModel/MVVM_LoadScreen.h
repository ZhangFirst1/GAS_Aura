// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "MVVM_LoadScreen.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FSlotSelected);

class UMVVM_LoadSlot;
/**
 * 
 */
UCLASS()
class AURA_API UMVVM_LoadScreen : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:
	// 初始化三个槽位
	void InitializeLoadSlots();

	// 选择槽位时触发的委托
	UPROPERTY(BlueprintAssignable)
	FSlotSelected SelectSlot;

	// LoadSlot的ModelView类，也就是单个槽位是用哪个图纸生成的
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UMVVM_LoadSlot> LoadSlotViewModelClass;

	// 通过下标获取槽位
	UFUNCTION(BlueprintCallable)
	UMVVM_LoadSlot* GetLoadSlotViewModelByIndex(int32 Index) const;

	// 一系列按钮
	UFUNCTION(BlueprintCallable)
	void NewSlotButtonPressed(int32 Slot, const FString& EnteredName);

	UFUNCTION(BlueprintCallable)
	void NewGameButtonPressed(int32 Slot);

	UFUNCTION(BlueprintCallable)
	void SelectSlotButtonPressed(int32 Slot);
	
	UFUNCTION(BlueprintCallable)
	void DeleteButtonPressed();

	UFUNCTION(BlueprintCallable)
	void PlayButtonPressed();

	void SetNumLoadSlots(int32 InNumLoadSlots);

	int32 GetNumLoadSlots() const { return NumLoadSlots; }
	// 加载数据
	void LoadData();
private:
	// 用一个 Map 把索引（0,1,2）和对应的槽位 ViewModel 存起来
	UPROPERTY()
	TMap<int32, UMVVM_LoadSlot*> LoadSlots;

	// 具体的三个槽位 ViewModel 实例
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_0;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_1;
	
	UPROPERTY()
	TObjectPtr<UMVVM_LoadSlot> LoadSlot_2;

	// 当前选择的槽位
	UPROPERTY()
	UMVVM_LoadSlot* SelectedSlot;

	// 总槽位数
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess="true"))
	int32 NumLoadSlots;
};
