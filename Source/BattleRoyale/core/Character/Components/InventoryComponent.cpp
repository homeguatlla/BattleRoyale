// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryComponent.h"

#include <cassert>

#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "BattleRoyale/core/PickableObjects/PickableObjectBase.h"
#include "BattleRoyale/core/PickableObjects/Ammo/Ammo.h"
#include "BattleRoyale/core/PickableObjects/Weapons/WeaponBase.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryItemInstance.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArray.h"
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
	mInventoryBag = CreateDefaultSubobject<UInventoryBag>("InventoryBag");
	mInventoryBag->SetMaxItems(MaxInventoryItems);
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
		//mInventoryArray.AddItem(item);
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	auto result = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	mInventoryBag->PerformActionForEachItem([&Channel, &Bunch, &RepFlags, &result](const FInventoryArrayItem& inventoryItem)-> bool
	{
		if(const auto inventoryItemInstance = inventoryItem.mInventoryItem)
		{
			result = Channel->ReplicateSubobject(inventoryItemInstance.GetObject(), *Bunch, *RepFlags);
		}
		return false;
	});
	return result;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, mInventoryBag);
	DOREPLIFETIME(UInventoryComponent, mEquippedItem);
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//DEBUG purposes only
	if(ConsoleShowInventory.GetValueOnGameThread() != 0)
	{
		mInventoryBag->PerformActionForEachItem([](const FInventoryArrayItem& inventoryItem) -> bool
		{
			const auto inventoryItemStaticData = inventoryItem.mInventoryItem->GetStaticData();
			assert(inventoryItemStaticData);
			GEngine->AddOnScreenDebugMessage(-1, 0, FColor::Blue, inventoryItemStaticData->GetItemName().ToString());
			return false;
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
	
	if(!HasItemEquipped() && pickableObject.GetObject()->IsA<AWeaponBase>())
	{
		return EquipItem(pickableObject);
	}
	else
	{
		//TODO si tiene un arma equipada, entonces desequipar, no sé si guardarla al inventario o no
		//y equipar la nueva.
	}
	
	//Save picked up object to the inventory.
	if(mInventoryBag->IsFull())
	{
		return false;	
	}
	
	mInventoryBag->AddItem(pickableObject->GetInventoryItemStaticData(), pickableObject->GetValue());
	if(const auto inventoryItemInstance = mInventoryBag->FindFirstItem(pickableObject->GetInventoryItemStaticData()))
	{
		inventoryItemInstance->OnUnEquipped();
		//Destroy the pickable object actor
		if(const auto object = Cast<APickableObjectBase>(pickableObject.GetObject()))
		{
			object->Destroy();
			return true;
		}
	}
	return false;
}

bool UInventoryComponent::DropObjectServer()
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	
	if(!character->HasAuthority())
	{
		return false;
	}

	//TODO hay que tener en cuenta que, en estos momentos el drop se hace del objeto equipado.
	//Si hacemos un unequip se irá de la mano a la mochila.
	//Si hacemos equip, irá de la mochila a la mano o del suelo a la mano
	//Si hacemos drop, irá de la mano al suelo y quizá de la mochila al suelo para evitar tener que pasar por la mano
	//ya que si ya tienes un objeto en la mano, no podrías hacer drop.
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

bool UInventoryComponent::HasAmmoOfType(EAmmoType ammoType) const
{
	const auto inventoryItemInstance = GetAmmoItemOfType(ammoType);
	
	return inventoryItemInstance != nullptr;
}

bool UInventoryComponent::HasItemOfType(TSubclassOf<UInventoryItemStaticData> itemStaticDataClassToFind) const
{
	return mInventoryBag->FindFirstItem(itemStaticDataClassToFind) != nullptr;
}

bool UInventoryComponent::HasLifeKid() const
{
	return false;
}

int UInventoryComponent::RemoveEnoughAmmo(EAmmoType ammoType, int ammoNeeded)
{
	int ammoRemoved = 0;
	auto inventoryItemInstance = GetAmmoItemOfType(ammoType);

	while(ammoRemoved < ammoNeeded && inventoryItemInstance != nullptr)
	{
		const auto ammoAvailable = inventoryItemInstance->GetValue();
		if(ammoRemoved + ammoAvailable >= ammoNeeded)
		{		
			const auto newAmmoValue = ammoAvailable - (ammoNeeded - ammoRemoved);
			if(newAmmoValue == 0)
			{
				mInventoryBag->RemoveFirstItem(inventoryItemInstance->GetStaticDataClass());
			}
			else
			{
				inventoryItemInstance->UpdateValue(newAmmoValue);
			}
			ammoRemoved = ammoNeeded;
		}
		else
		{
			mInventoryBag->RemoveFirstItem(inventoryItemInstance->GetStaticDataClass());
			ammoRemoved += ammoAvailable;
			inventoryItemInstance = GetAmmoItemOfType(ammoType);
		}
	}
	
	return ammoRemoved;
}

void UInventoryComponent::PerformActionForEachInventoryItem(
	const std::function<bool (const FInventoryArrayItem& inventoryItem)>& action) const
{
	mInventoryBag->PerformActionForEachItem(action);
}

TScriptInterface<IIInventoryItemInstance> UInventoryComponent::GetAmmoItemOfType(EAmmoType ammoType) const
{
	TScriptInterface<IIInventoryItemInstance> inventoryItemInstanceFound = nullptr;
	bool foundAmmoInTheInventory = false;
	
	PerformActionForEachInventoryItem(
		[&ammoType, &foundAmmoInTheInventory, &inventoryItemInstanceFound](const FInventoryArrayItem& inventoryItem) -> bool
		{
			const auto staticData = inventoryItem.mInventoryItem->GetStaticData();
			const auto objectClass = staticData->GetPickupObjectClass();
			const auto pickableObject = objectClass.GetDefaultObject();
			if(pickableObject->IsA<AAmmo>())
			{
				const auto ammo = Cast<AAmmo>(pickableObject);
				if(ammo->GetAmmoType() == ammoType)
				{
					foundAmmoInTheInventory = true;
					inventoryItemInstanceFound = inventoryItem.mInventoryItem;
					return true;
				}
			}
			return false;
		});
	
	return inventoryItemInstanceFound;
}

void UInventoryComponent::OnInventoryKeyPressed()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetOwner()->GetGameInstance());
	check(gameInstance);

	if(mIsInventoryShown)
	{
		gameInstance->GetEventDispatcher()->OnHideInventoryScreen.Broadcast();
	}
	else
	{
		gameInstance->GetEventDispatcher()->OnShowInventoryScreen.Broadcast(mInventoryBag);
	}
	mIsInventoryShown = !mIsInventoryShown;
}
