// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "InventoryItemStaticData.h"
#include "InventoryArrayItem.h"
#include "InventoryArray.generated.h"

UCLASS()
class UInventoryArray : public UNetworkObject//public FFastArraySerializer
{
	GENERATED_BODY()

public:
	void AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass, int value);
	void RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	TScriptInterface<IIInventoryItemInstance> FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass);

	void PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const;
	int Num() const { return mItems.Num(); }
	UInventoryArrayItem* GetItemByIndex(int index) const;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	void Clear();

	/*
	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FInventoryArrayItem, FInventoryArray>(mItems, DeltaParams, *this);
	}*/

	UPROPERTY(Replicated)
	TArray<UInventoryArrayItem*> mItems;
};
/*
template<>
struct TStructOpsTypeTraits<FInventoryArray> : public TStructOpsTypeTraitsBase2<FInventoryArray>
{
	enum { WithNetDeltaSerializer = true };
};*/