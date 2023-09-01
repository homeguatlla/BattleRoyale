// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryArray.h"
#include "UObject/Object.h"
#include "InventoryBag.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BATTLEROYALE_API UInventoryBag : public UObject

{
	GENERATED_BODY()

public:
	UInventoryBag();
	
	void AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);
	void RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	void SetMaxItems(int max) { mMaxItems = max; }
	UInventoryItemInstance* FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	void PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action);
	UFUNCTION(BlueprintCallable)
	int Num() const { return mInventoryArray.Num(); }
	UFUNCTION(BlueprintCallable)
	bool IsFull() const { return mInventoryArray.Num() >= mMaxItems; }
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUserWidget> GetItemWidgetClassByIndex(int index);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FInventoryArray mInventoryArray;

	int mMaxItems = 5;
};
