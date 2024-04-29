// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHUD.generated.h"

class UInventoryBagVisual;
class IPickupObject;
struct FInventoryArray;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API AInventoryHUD : public AActor
{
	GENERATED_BODY()

public:	
	void Initialize(int maxInventoryItems, int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> widgetClasses);

private:
	void BindToDelegate();
	
	UFUNCTION()
	void OnShowInventoryScreen(const TScriptInterface<IIInventoryBag> inventoryBag, const TScriptInterface<IPickupObject>& equippedObject);

	UFUNCTION()
	void OnHideInventoryScreen();

	UFUNCTION()
	void OnRefreshInventory(const TScriptInterface<IIInventoryBag> inventoryBag);
public:
	UPROPERTY()
	UUserWidget* mHUDWidget;

	UPROPERTY()
	UInventoryBagVisual* inventoryBagVisual;
};
