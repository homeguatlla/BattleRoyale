// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInventoryHUD.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIInventoryHUD : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIInventoryHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnShowInventoryScreen(const UInventoryBagVisual* items);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnHideInventoryScreen();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnRefreshInventory(const UInventoryBagVisual* items);
};
