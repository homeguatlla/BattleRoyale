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
UCLASS()
class BATTLEROYALE_API UInventoryBag : public UObject, public IIInventoryBag
{
	GENERATED_BODY()

public:
	UInventoryBag();
	
	virtual void AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value) override;
	virtual void RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	
	virtual void SetMaxItems(int max) override { mMaxItems = max; }
	virtual TScriptInterface<IIInventoryItemInstance> FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	
	virtual bool ExistItemWithID(int ID) const override;
	
	virtual void PerformActionForEachItem(const std::function<bool(const FInventoryArrayItem& inventoryItem)>& action) const override;
	virtual int Num() const override { return mInventoryArray.Num(); }
	virtual bool IsFull() const override { return mInventoryArray.Num() >= mMaxItems; }
	virtual bool IsEmpty() const override { return mInventoryArray.Num() <= 0; }
	TSubclassOf<UUserWidget> GetItemWidgetClassByIndex(int index) const;
	
protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	FInventoryArray mInventoryArray;

	int mMaxItems = 5;
};
