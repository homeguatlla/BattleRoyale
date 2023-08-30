// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHUD.generated.h"

struct FInventoryArray;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API AInventoryHUD : public AActor
{
	GENERATED_BODY()

public:	
	void Initialize(int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> widgetClasses);

private:
	void BindToDelegate();
	
	UFUNCTION()
	void OnShowInventoryScreen(const FInventoryArray& items);

	UFUNCTION()
	void OnHideInventoryScreen();

	UFUNCTION()
	void OnRefreshInventory(const FInventoryArray& items);
public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
