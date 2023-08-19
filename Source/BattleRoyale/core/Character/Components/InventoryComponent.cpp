// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include <cassert>

#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/General/IPickupObject.h"
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
		mInventoryArray.AddItemOfClass(item);
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
	DOREPLIFETIME(UInventoryComponent, mEquippedItem);
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

bool UInventoryComponent::PickupObjectServer(TScriptInterface<IPickupObject> pickableObject)
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	
	if(!character->HasAuthority())
	{
		return false;
	}
	
	check(pickableObject);
	if(pickableObject->IsEquipped())
	{
		return false;
	}
	
	if(!HasItemEquipped())
	{
		return EquipItem(pickableObject);
	}
	else
	{
		//TODO si tiene un arma equipada, entonces desequipar, no sé si guardarla al inventario o no
		//y equipar la nueva.
	}
	
	//TODO guardar en el inventario
	//Crear instancia del item y guardarlo en el inventoryArray
	
	return false;
}

bool UInventoryComponent::DropObjectServer()
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	
	if(!character->HasAuthority())
	{
		return false;
	}

	if(!HasItemEquipped())
	{
		return false;	
	}
	
	const TScriptInterface<IPickupObject> pickupObject = GetEquippedItem();
	check(pickupObject.GetObject());
	
	pickupObject->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	pickupObject->OnDropped();
	mEquippedItem = nullptr;
	OnDroppedPickableObject.Broadcast();
	
	return true;
}

bool UInventoryComponent::EquipItem(TScriptInterface<IPickupObject> pickableObject)
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	//attach it to hand
	const auto isAttached = pickableObject->AttachToComponent(
				character->GetMesh(),
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				character->GetRightHandSocketName());
	if(!isAttached)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::Equip] pickup object not attached to the character"), *GetName());
		return false;
	}

	pickableObject->SetCharacterOwner(character);
	pickableObject->OnEquipped();
	mEquippedItem = pickableObject;
	OnEquippedPickableObjectDelegate.Broadcast(pickableObject);
	
	return true;
}

TScriptInterface<IPickupObject> UInventoryComponent::GetEquippedItem() const
{
	return mEquippedItem;
}

