// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include <cassert>

#include "BattleRoyale/core/Utils/Inventory/InventoryItemInstance.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArrayItem.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryItemStaticData.h"
#include "Engine/ActorChannel.h"
#include "Net/UnrealNetwork.h"


static TAutoConsoleVariable<bool> ConsoleShowInventory(
	TEXT("BR.ShowInventory"),
	false,
	TEXT("Draws debug info about inventory")
	TEXT(" false: off/n")
	TEXT(" true:   on/n"),
	ECVF_Cheat);

UInventoryComponent::UInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
	SetIsReplicatedByDefault(true);
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if(!GetOwner()->HasAuthority())
	{
		return;
	}

	//Adding default items to player
	for(const auto item : DefaultItems)
	{
		mInventoryArray.AddItem(item);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	auto result = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	mInventoryArray.PerformActionForEachItem([&Channel, &Bunch, &RepFlags, &result](const FInventoryArrayItem& inventoryItem)
	{
		if(const auto inventoryItemInstance = inventoryItem.mInventoryItem)
		{
			result = Channel->ReplicateSubobject(inventoryItemInstance, *Bunch, *RepFlags);
		}
	});
	return result;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, mInventoryArray);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//DEBUG purposes only
	if(ConsoleShowInventory.GetValueOnGameThread() != 0)
	{
		mInventoryArray.PerformActionForEachItem([](const FInventoryArrayItem& inventoryItem)
		{
			const auto inventoryItemStaticData = inventoryItem.mInventoryItem->GetStaticData();
			assert(inventoryItemStaticData);
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, inventoryItemStaticData->GetItemName().ToString());
		});
	}
}

