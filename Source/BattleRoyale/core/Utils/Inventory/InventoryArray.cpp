// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryArray.h"
#include "InventoryArrayItem.h"
#include "InventoryItemInstance.h"

void FInventoryArray::AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass, int _value)
{
	auto& item = mItems.AddDefaulted_GetRef();
	item.mInventoryItem = NewObject<UInventoryItemInstance>();
	item.mInventoryItem->Initialize(itemClass, _value);
	MarkItemDirty(item);
}

void FInventoryArray::RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	for(auto it = mItems.CreateConstIterator(); it; ++it)
	{
		if(it->mInventoryItem && it->mInventoryItem.GetObject()->IsA(itemClass))
		{
			//it.RemoveCurrent();
			const auto index = it.GetIndex();
			mItems.RemoveAt(index);
			MarkArrayDirty();
			break;
		}
	}
}

TScriptInterface<IIInventoryItemInstance> FInventoryArray::FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
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

void FInventoryArray::PerformActionForEachItem(const std::function<bool(const FInventoryArrayItem& inventoryItem)>& action)
{
	for(auto&& item : mItems)
	{
		if(action(item))
		{
			break;
		}
	}
}

FInventoryArrayItem FInventoryArray::GetItemByIndex(int index) const
{
	if(index < 0 || index >= mItems.Num())
	{
		return {};
	}

	return mItems[index];
}

void FInventoryArray::Clear()
{
	mItems.Empty();
}
