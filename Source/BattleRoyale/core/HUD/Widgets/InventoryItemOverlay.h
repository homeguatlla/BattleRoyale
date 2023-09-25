// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Overlay.h"
#include "InventoryItemOverlay.generated.h"

/**
 * 
 */
UCLASS(Blueprintable, BlueprintType)
class BATTLEROYALE_API UInventoryItemOverlay : public UOverlay
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	const TSubclassOf<UUserWidget> DefaultWidgetClass;

	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const { return mIsEmpty; }
	UFUNCTION(BlueprintCallable)
	void SetItem(int value, const TSubclassOf<UUserWidget>& widget);
	UFUNCTION(BlueprintCallable)
	void RemoveItem();
	
private:
	UPROPERTY()
	TSubclassOf<UUserWidget> CurrentWidgetClass;

	bool mIsEmpty = true;
};
