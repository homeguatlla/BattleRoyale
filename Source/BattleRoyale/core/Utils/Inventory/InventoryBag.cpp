// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryBag.h"

#include "IInventoryItemInstance.h"
#include "InventoryItemInstance.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArrayItem.h"

UInventoryBag::UInventoryBag()
{
	mInventoryArray = NewObject<UInventoryArray>();
}

TScriptInterface<IIInventoryItemInstance> UInventoryBag::AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value, int value2)
{
	check(itemClass);
	return mInventoryArray->AddItemOfClass(itemClass, value, value2);
}

void UInventoryBag::RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	mInventoryArray->RemoveFirstItemOfClass(itemClass);
}

void UInventoryBag::RemoveItem(TScriptInterface<IIInventoryItemInstance> item)
{
	mInventoryArray->RemoveItem(item);
}

void UInventoryBag::RemoveItemByID(int ID)
{
	mInventoryArray->RemoveItemByID(ID);
}

TScriptInterface<IIInventoryItemInstance> UInventoryBag::FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass)
{
	return mInventoryArray->FindFirstItemOfClass(itemClass);
}

bool UInventoryBag::ExistItemWithID(int ID) const
{
	for(int i = 0; i < Num(); ++i)
	{
		const auto item = mInventoryArray->GetItemByIndex(i);
		if(item->GetID() == ID)
		{
			return true;
		}
	}
	
	return false;
}

TScriptInterface<IIInventoryItemInstance> UInventoryBag::FindItemWithID(int ID)
{
	for(int i = 0; i < Num(); ++i)
	{
		const auto item = mInventoryArray->GetItemByIndex(i);
		if(item->GetID() == ID)
		{
			return item->mInventoryItem;
		}
	}
	return nullptr;
}

void UInventoryBag::PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const
{
	mInventoryArray->PerformActionForEachItem(action);
}

TSubclassOf<UUserWidget> UInventoryBag::GetItemWidgetClassByIndex(int index) const
{
	if(index < 0 || index >= mInventoryArray->Num())
	{
		return nullptr;
	}

	const auto item = mInventoryArray->GetItemByIndex(index);
	return item->mInventoryItem->GetStaticData()->GetItemWidgetClass();
}

void UInventoryBag::AddReplicatedSubObject(UActorComponent* owner)
{
	PerformActionForEachItem([owner](const UInventoryArrayItem* inventoryItem)-> bool
	{
		if(const auto inventoryItemInstance = inventoryItem->mInventoryItem)
		{
			owner->AddReplicatedSubObject(inventoryItemInstance);
		}
		return false;
	});
}

bool UInventoryBag::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool result = Channel->ReplicateSubobject(mInventoryArray, *Bunch, *RepFlags);
	result |= mInventoryArray->ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	return result;
}

/*
bool UInventoryBag::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool result = false;
	PerformActionForEachItem([&Channel, &Bunch, &RepFlags, &result](const FInventoryArrayItem& inventoryItem)-> bool
	{
		if(const auto inventoryItemInstance = inventoryItem.mInventoryItem)
		{
			result |= Channel->ReplicateSubobject(inventoryItemInstance, *Bunch, *RepFlags);
		}
		return false;
	});
	
	return result;
}*/

void UInventoryBag::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryBag, mInventoryArray);
}
