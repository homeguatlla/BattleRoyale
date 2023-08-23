// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryArray.h"
#include "InventoryArrayItem.h"
#include "InventoryItemInstance.h"

void FInventoryArray::AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	auto& item = mItems.AddDefaulted_GetRef();
	item.mInventoryItem = NewObject<UInventoryItemInstance>();
	item.mInventoryItem->Initialize(itemClass);
	MarkItemDirty(item);
}

void FInventoryArray::RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	for(auto it = mItems.CreateConstIterator(); it; ++it)
	{
		if(it->mInventoryItem && it->mInventoryItem->IsA(itemClass))
		{
			//it.RemoveCurrent();
			const auto index = it.GetIndex();
			mItems.RemoveAt(index);
			MarkArrayDirty();
			break;
		}
	}
}

UInventoryItemInstance* FInventoryArray::FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	for(auto&& item : mItems)
	{
		if(item.mInventoryItem->GetStaticData()->IsA(itemClass))
		{
			return item.mInventoryItem;
		}
	}
	return nullptr;
}

void FInventoryArray::PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action)
{
	for(auto&& item : mItems)
	{
		action(item);
	}
}
