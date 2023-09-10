// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBagMock.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArrayItem.h"

void UInventoryBagMock::AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value)
{
}

void UInventoryBagMock::RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
}

bool UInventoryBagMock::ExistItemWithID(int ID) const
{
	for(auto&& item : mItems)
	{
		if(item.id == ID)
		{
			return true;
		}
	}
	return false;
}

void UInventoryBagMock::SetMaxItems(int max)
{
	mMaxItems = max;
}

TScriptInterface<IIInventoryItemInstance> UInventoryBagMock::FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	return nullptr;
}

void UInventoryBagMock::PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action)
{
	for(auto&& item : mItems)
	{
		FInventoryArrayItem inventoryItem;
		inventoryItem.SetID(item.id);
		//inventoryItem.mInventoryItem = item.data;
		action(inventoryItem);
	}
}

int UInventoryBagMock::Num() const
{
	return mItems.Num();
}

bool UInventoryBagMock::IsFull() const
{
	return mItems.Num() == mMaxItems;
}

bool UInventoryBagMock::IsEmpty() const
{
	return mItems.Num() <= 0;
}
