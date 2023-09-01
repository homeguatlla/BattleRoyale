// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBag.h"
#include "Net/UnrealNetwork.h"

UInventoryBag::UInventoryBag()
{
	mInventoryArray.Clear();
}

void UInventoryBag::AddItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	mInventoryArray.AddItemOfClass(itemClass);
}

void UInventoryBag::RemoveFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	mInventoryArray.RemoveFirstItemOfClass(itemClass);
}

UInventoryItemInstance* UInventoryBag::FindFirstItemOfClass(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	return mInventoryArray.FindFirstItemOfClass(itemClass);
}

void UInventoryBag::PerformActionForEachItem(std::function<void(const FInventoryArrayItem& inventoryItem)> action)
{
	mInventoryArray.PerformActionForEachItem(action);
}

TSubclassOf<UUserWidget> UInventoryBag::GetItemWidgetClassByIndex(int index)
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
