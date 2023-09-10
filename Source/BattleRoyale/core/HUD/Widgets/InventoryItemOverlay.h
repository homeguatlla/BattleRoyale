// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInventoryItemOverlay.h"
#include "Components/Overlay.h"
#include "InventoryItemOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UInventoryItemOverlay : public UOverlay
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const { return mIsEmpty; }
	UFUNCTION(BlueprintCallable)
	void SetItem(int value, const TSubclassOf<UUserWidget>& widget);
	UFUNCTION(BlueprintCallable)
	void RemoveItem();
	
private:
	UPROPERTY(EditDefaultsOnly)
	const TSubclassOf<UUserWidget> DefaultWidgetClass;

	UPROPERTY()
	TSubclassOf<UUserWidget> CurrentWidgetClass;

	bool mIsEmpty = true;
};
