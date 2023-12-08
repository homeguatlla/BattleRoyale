// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemVisual.h"
#include "UObject/Object.h"
#include "InventoryBagVisual.generated.h"

class IIInventoryBag;

/**
 * 
 */
UCLASS(BlueprintType)
class BATTLEROYALE_API UInventoryBagVisual : public UObject
{
	GENERATED_BODY()

public:
	
	//num items that can support
	void Initialize(int max);
	void Refresh(const TScriptInterface<IIInventoryBag> inventoryBag);

	UFUNCTION(BlueprintCallable)
	FInventoryItemVisual GetItemByIndex(int index) const;

	UFUNCTION(BlueprintCallable)
	int FindItemByID(int ID) const;
	
	UFUNCTION(BlueprintCallable)
	int Num() const;

	UFUNCTION(BlueprintCallable)
	bool IsEmpty() const;
	
private:
	int mMaxItems = 0;
	TArray<FInventoryItemVisual> mItems;
};
