// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "IInventoryHUD.generated.h"

class APickableObjectBase;
class UInventoryBagVisual;

// This class does not need to be modified.
UINTERFACE()
class BATTLEROYALE_API UIInventoryHUD : public UInterface
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
	//OnShowInventoryOnScreen second parameter is an & to the tscriptinterface otherwise will not compile.(not used but as example)
	//void OnShowInventoryOnScreen(const UInventoryBagVisual* items, const TScriptInterface<IPickableObject>& equippedObject);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnShowInventoryOnScreen(const UInventoryBagVisual* items, const APickableObjectBase* equippedObject);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnTest();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnHideInventoryOnScreen();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnRefreshInventory(const UInventoryBagVisual* items);
};
