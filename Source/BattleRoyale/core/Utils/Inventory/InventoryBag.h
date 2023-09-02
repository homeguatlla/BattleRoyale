// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInventoryBag.h"
#include "InventoryArray.h"
#include "UObject/Object.h"
#include "InventoryBag.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BATTLEROYALE_API UInventoryBag : public UObject, public IIInventoryBag
{
	GENERATED_BODY()

public:
	UInventoryBag();
	
	virtual void AddItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	virtual void RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;

	virtual void SetMaxItems(int max) override { mMaxItems = max; }
	virtual UInventoryItemInstance* FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;

	virtual void PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action) override;
	UFUNCTION(BlueprintCallable)
	virtual int Num() const override { return mInventoryArray.Num(); }
	UFUNCTION(BlueprintCallable)
	virtual bool IsFull() const override { return mInventoryArray.Num() >= mMaxItems; }
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUserWidget> GetItemWidgetClassByIndex(int index);
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FInventoryArray mInventoryArray;

	int mMaxItems = 5;
};
