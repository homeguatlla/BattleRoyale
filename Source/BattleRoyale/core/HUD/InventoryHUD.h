// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Blueprint/UserWidget.h"
#include "InventoryHUD.generated.h"

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
	void OnShowInventoryScreen();

	UFUNCTION()
	void OnHideInventoryScreen();
	
public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
