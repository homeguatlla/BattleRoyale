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
	
	void AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);
	void RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	UInventoryItemInstance* FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);
	
	void PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action);
	
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