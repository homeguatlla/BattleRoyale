// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBagVisual.h"
#include "IInventoryBag.h"
#include "InventoryArrayItem.h"
#include "InventoryItemStaticData.h"
#include "InventoryItemVisual.h"

void UInventoryBagVisual::Initialize(int max)
{
	mMaxItems = max;
	for(auto i = 0; i < mMaxItems; ++i)
	{
		const auto itemVisual = FInventoryItemVisual(
				-1,
				0,
				false,
				nullptr);
		mItems.Add(itemVisual);
	}
}

void UInventoryBagVisual::Refresh(const TScriptInterface<IIInventoryBag> inventoryBag)
{
	//Remove elements that are not in the bag
	for(int i = mItems.Num()-1 ; i >= 0 ; --i)
	{
		auto& itemVisual = mItems[i];
		if(!inventoryBag->ExistItemWithID(itemVisual.id))
		{
			itemVisual.Clear();
		}
	}

	int i = 0;
	inventoryBag->PerformActionForEachItem([this, &i](UInventoryArrayItem* inventoryItem) -> bool
	{
		const auto index = FindItemByID(inventoryItem->GetID());
		if(index == -1) //not found
		{
			//Find first empty space. Must be space for all.			
			while(i < mItems.Num() && !mItems[i].IsEmpty())
			{
				i++;
			}

			const auto itemVisual = FInventoryItemVisual(
				inventoryItem->GetID(),
				inventoryItem->mInventoryItem->GetValue(),
				false,
				inventoryItem->mInventoryItem->GetStaticData()->GetItemWidgetClass());
			
			mItems[i] = itemVisual;
		}
		else
		{
			mItems[index] = inventoryItem;
		}
		return false;
	});
}

FInventoryItemVisual UInventoryBagVisual::GetItemByIndex(int index) const
{
	if(index < 0 || index >= mItems.Num())
	{
		return {};
	}
	return mItems[index];
}

int UInventoryBagVisual::FindItemByID(int ID) const
{
	for(int i = 0; i < mItems.Num(); ++i)
	{
		if(mItems[i].id == ID)
		{
			return i;
		}
	}
	return -1;
}

int UInventoryBagVisual::Num() const
{
	return mItems.Num();
}

bool UInventoryBagVisual::IsEmpty() const
{
	for(auto item : mItems)
	{
		if(!item.IsEmpty())
		{
			return false;
		}
	}
	return true;
}
