// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBag.h"

#include "IInventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

UInventoryBag::UInventoryBag()
{
	mInventoryArray.Clear();
}

void UInventoryBag::AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value)
{
	mInventoryArray.AddItemOfClass(itemClass, value);
}

void UInventoryBag::RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	mInventoryArray.RemoveFirstItemOfClass(itemClass);
}

TScriptInterface<IIInventoryItemInstance> UInventoryBag::FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	return mInventoryArray.FindFirstItemOfClass(itemClass);
}

bool UInventoryBag::ExistItemWithID(int ID) const
{
	for(int i = 0; i < Num(); ++i)
	{
		const auto item = mInventoryArray.GetItemByIndex(i);
		if(item.GetID() == ID)
		{
			return true;
		}
	}
	
	return false;
}

void UInventoryBag::PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action)
{
	mInventoryArray.PerformActionForEachItem(action);
}

TSubclassOf<UUserWidget> UInventoryBag::GetItemWidgetClassByIndex(int index) const
{
	if(index < 0 || index >= mInventoryArray.Num())
	{
		return nullptr;
	}

	const auto item = mInventoryArray.GetItemByIndex(index);
	return item.mInventoryItem->GetStaticData()->GetItemWidgetClass();
}

void UInventoryBag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryBag, mInventoryArray);
}
