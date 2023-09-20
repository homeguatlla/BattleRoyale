// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryItemStaticData.h"
#include "InventoryArrayItem.h"
#include "InventoryArray.generated.h"

USTRUCT(BlueprintType)
struct FInventoryArray : public FFastArraySerializer
{
	GENERATED_BODY()
	
	void AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass, int value);
	void RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	TScriptInterface<IIInventoryItemInstance> FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	void PerformActionForEachItem(const std::function<bool(const FInventoryArrayItem& inventoryItem)>& action);
	int Num() const { return mItems.Num(); }
	FInventoryArrayItem GetItemByIndex(int index) const;

	void Clear();
	
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryArrayItem, FInventoryArray>(mItems, DeltaParams, *this);
	}

	UPROPERTY()
	TArray<FInventoryArrayItem> mItems;
};

template<>
struct TStructOpsTypeTraits<FInventoryArray> : public TStructOpsTypeTraitsBase2<FInventoryArray>
{
	enum { WithNetDeltaSerializer = true };
};