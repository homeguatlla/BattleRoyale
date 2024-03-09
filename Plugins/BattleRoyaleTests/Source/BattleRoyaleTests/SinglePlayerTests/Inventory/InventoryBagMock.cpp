// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBagMock.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArrayItem.h"

TScriptInterface<IIInventoryItemInstance> UInventoryBagMock::AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value1, int value2)
{
	return nullptr;
}

void UInventoryBagMock::RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
}

void UInventoryBagMock::RemoveItem(TScriptInterface<IIInventoryItemInstance> item)
{
}

void UInventoryBagMock::RemoveItemByID(int ID)
{
}

bool UInventoryBagMock::ExistItemWithID(int ID) const
{
	for(auto&& item : mItems)
	{
		if(item.mId == ID)
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

TScriptInterface<IIInventoryItemInstance> UInventoryBagMock::FindItemWithID(int ID)
{
	return nullptr;
}

void UInventoryBagMock::PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const
{
	for(auto&& item : mItems)
	{
		const auto inventoryItem = NewObject<UInventoryArrayItem>();
		inventoryItem->SetID(item.mId);
		inventoryItem->mInventoryItem = nullptr;
		if(action(inventoryItem))
		{
			break;
		}
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
