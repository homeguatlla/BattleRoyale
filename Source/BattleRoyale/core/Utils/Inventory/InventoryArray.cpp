﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryArray.h"
#include "EngineMinimal.h"
#include "InventoryArrayItem.h"
#include "InventoryItemInstance.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"

TScriptInterface<IIInventoryItemInstance> UInventoryArray::AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass, int value, int value2)
{
	//auto& item = mItems.AddDefaulted_GetRef();
	const auto item = NewObject<UInventoryArrayItem>();
	item->mInventoryItem = NewObject<UInventoryItemInstance>();
	item->mInventoryItem->Initialize(itemClass, value, value2);
	mItems.Add(item);

	return item->mInventoryItem;
	//MarkItemDirty(item);
}

void UInventoryArray::RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	int32 itemIndex = -1;

	for(auto index = 0; index < mItems.Num(); ++index)
	{
		const auto item = mItems[index];
		if(item->mInventoryItem && item->mInventoryItem->GetStaticDataClass() == itemClass)
		{
			itemIndex = index;
			break;
		}
	}

	if(mItems.IsValidIndex(itemIndex))
	{
		mItems.RemoveAt(itemIndex);
	}
	/*for(auto it = mItems.CreateConstIterator(); it; ++it)
	{
		if(item->mInventoryItem && item->mInventoryItem->GetStaticDataClass() == itemClass)
		{
			//it.RemoveCurrent();
			//const auto index = item.GetIndex();
			mItems.Remove(item);
			//mItems.RemoveAt(index);
			//MarkArrayDirty();
			break;
		}
	}*/
}

void UInventoryArray::RemoveItem(const TScriptInterface<IIInventoryItemInstance> itemInstance)
{
	for(int i = 0; i < mItems.Num() ; ++i)
	{
		const auto inventoryItemInstance = Cast<UInventoryItemInstance>(itemInstance.GetObject());
		if(mItems[i]->mInventoryItem == inventoryItemInstance)
		{
			mItems.RemoveAt(i);
			break;
		}
	}
}

void UInventoryArray::RemoveItemByID(int ID)
{
	for(int i = 0; i < mItems.Num() ; ++i)
	{
		if(mItems[i]->GetID() == ID)
		{
			mItems.RemoveAt(i);
			break;
		}
	}
}

TScriptInterface<IIInventoryItemInstance> UInventoryArray::FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	for(auto&& item : mItems)
	{
		if(item->mInventoryItem->GetStaticData()->IsA(itemClass))
		{
			return item->mInventoryItem;
		}
	}
	return nullptr;
}

void UInventoryArray::PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const
{
	for(auto&& item : mItems)
	{
		if(action(item))
		{
			break;
		}
	}
}

UInventoryArrayItem* UInventoryArray::GetItemByIndex(int index) const
{
	if(index < 0 || index >= mItems.Num())
	{
		return {};
	}

	return mItems[index];
}

void UInventoryArray::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryArray, mItems);
}

bool UInventoryArray::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool result = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for(auto&& item : mItems)
	{
		result |= Channel->ReplicateSubobject(item, *Bunch, *RepFlags);
		result |= item->ReplicateSubobjects(Channel, Bunch, RepFlags);
	}

	return result;
}

void UInventoryArray::Clear()
{
	mItems.Empty();
}
