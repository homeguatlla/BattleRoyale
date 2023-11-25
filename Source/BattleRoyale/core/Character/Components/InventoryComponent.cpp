﻿// Fill out your copyright notice in the Description page of Project Settings.


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
	SetIsReplicatedByDefault(true); //esto no sé si hace falta pero estaba en el curso.
	//bReplicateUsingRegisteredSubObjectList = true;
}

void UInventoryComponent::InitializeComponent()
{
	Super::InitializeComponent();
	
	if(!GetOwner()->HasAuthority())
	{
		return;
	}
	
	//If I use a CreateDefaultSubObjects in the constructor for this UObject InventoryBag. Seems everytime I start
	//a new game, the inventoryBag is like being copied from a default data which contains the last game data.
	//So, the inventoryBag contains 2 elements in the last game, after creation in a new game is filled up with 2 elements.
	//Seems that is not correct initialize a UObject with a CreateDefaultSubObjects
	//If, I use NewObject<UInventoryBag> in the constructor, the inventoryBag is different than nullptr but,
	//later, is null. So, we can not call NewObjects in the constructor. Seems that, when executing the constructor
	//the object is not loaded yet and then, one loaded in the BeginPlay or the InitializeComponent can be reset.
	
	mInventoryBag = NewObject<UInventoryBag>(this);
	mInventoryBag->SetMaxItems(MaxInventoryItems);
	//AddReplicatedSubObject(mInventoryBag);
	
	//Adding default items to player
	for(const auto item : DefaultItems)
	{
		const auto itemDefault = item->GetDefaultObject<UInventoryItemStaticData>();
		mInventoryBag->AddItem(item, itemDefault->GetValue());
	}
}

bool UInventoryComponent::ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	auto result = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	result |= Channel->ReplicateSubobject(mInventoryBag, *Bunch, *RepFlags);
	result |= mInventoryBag->ReplicateSubobjects(Channel, Bunch, RepFlags);
	
	return result;
}

void UInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UInventoryComponent, mInventoryBag);
	DOREPLIFETIME(UInventoryComponent, mEquippedObject);
}

void UInventoryComponent::BeginDestroy()
{
	Super::BeginDestroy();

	if(mInventoryBag)
	{
		//RemoveReplicatedSubObject(mInventoryBag);
	}
}

void UInventoryComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                        FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	//DEBUG purposes only
	if(ConsoleShowInventory.GetValueOnGameThread() != 0)
	{
		mInventoryBag->PerformActionForEachItem([](UInventoryArrayItem* inventoryItem) -> bool
		{
			const auto inventoryItemStaticData = inventoryItem->mInventoryItem->GetStaticData();
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

	NotifyIfPickedUpObjectIsAmmo(pickableObject);
	GetOwner()->ForceNetUpdate();
	ClientNotifyPickedUpObject(Cast<APickableObjectBase>(pickableObject.GetObject()));
	
	//Once the pickable object has been saved we can remove it from the world.
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
	mEquippedObject = nullptr;
	OnDroppedPickableObjectDelegate.Broadcast();
	
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
	mEquippedObject = pickableObject;
	OnEquippedPickableObjectDelegate.Broadcast(pickableObject);
	
	NotifyEquippedWeapon(mEquippedObject);

	return true;
}

TScriptInterface<IPickupObject> UInventoryComponent::GetEquippedItem() const
{
	return mEquippedObject;
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

int UInventoryComponent::GetTotalAmmoOfType(EAmmoType ammoType) const
{
	int totalAmmo = 0;
	
	PerformActionForEachInventoryItem(
		[&ammoType, &totalAmmo](UInventoryArrayItem* inventoryItem) -> bool
		{
			const auto staticData = inventoryItem->mInventoryItem->GetStaticData();
			const auto objectClass = staticData->GetPickupObjectClass();
			const auto defaultPickableObject = objectClass.GetDefaultObject();
			if(defaultPickableObject->IsA<AAmmo>())
			{
				const auto ammo = Cast<AAmmo>(defaultPickableObject);
				if(ammo->GetAmmoType() == ammoType)
				{
					totalAmmo += inventoryItem->mInventoryItem->GetValue();
				}
			}
			return false;
		});

	return totalAmmo;
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
	const std::function<bool (UInventoryArrayItem* inventoryItem)>& action) const
{
	mInventoryBag->PerformActionForEachItem(action);
}

TScriptInterface<IIInventoryItemInstance> UInventoryComponent::GetAmmoItemOfType(EAmmoType ammoType) const
{
	TScriptInterface<IIInventoryItemInstance> inventoryItemInstanceFound = nullptr;
	bool foundAmmoInTheInventory = false;
	
	PerformActionForEachInventoryItem(
		[&ammoType, &foundAmmoInTheInventory, &inventoryItemInstanceFound](UInventoryArrayItem* inventoryItem) -> bool
		{
			const auto staticData = inventoryItem->mInventoryItem->GetStaticData();
			const auto objectClass = staticData->GetPickupObjectClass();
			const auto pickableObject = objectClass.GetDefaultObject();
			if(pickableObject->IsA<AAmmo>())
			{
				const auto ammo = Cast<AAmmo>(pickableObject);
				if(ammo->GetAmmoType() == ammoType)
				{
					foundAmmoInTheInventory = true;
					inventoryItemInstanceFound = inventoryItem->mInventoryItem;
					return true;
				}
			}
			return false;
		});
	
	return inventoryItemInstanceFound;
}

void UInventoryComponent::OnRep_EquippedObject() const
{
	if(mEquippedObject == nullptr)
	{
		OnDroppedPickableObjectDelegate.Broadcast();	
	}
	else
	{
		OnEquippedPickableObjectDelegate.Broadcast(mEquippedObject);
		NotifyEquippedWeapon(mEquippedObject);
	}
}

void UInventoryComponent::OnRep_InventoryBag() const
{
	
}

void UInventoryComponent::ClientNotifyPickedUpObject_Implementation(APickableObjectBase* pickedUpObject) const
{
	//Here we can NOT call NotifyIfPickedUpObjectIsAmmo because GetTotalAmmoOfType is not taking into account
	//the object just server has picked up because is not replicated to the client yet.
	//so, as we are receiving the object by parameter we can GetTotalAmmo and add the ammo of the object
	if(pickedUpObject->IsA<AAmmo>())
	{
		const auto ammo = Cast<AAmmo>(pickedUpObject);
		OnPickedUpAmmoDelegate.Broadcast(ammo->GetAmmoType(), GetTotalAmmoOfType(ammo->GetAmmoType()) + ammo->GetValue());
	}
}

void UInventoryComponent::NotifyEquippedWeapon(TScriptInterface<IPickupObject> pickableObject) const
{
	//Specific case, when weapon we notify a delegate for weapons.
	if(pickableObject.GetObject()->Implements<UWeapon>())
	{
		const TScriptInterface<IWeapon> weapon = pickableObject.GetObject();
		const auto ammoType = weapon->GetAmmoType();
		OnEquippedWeaponDelegate.Broadcast(weapon , GetTotalAmmoOfType(ammoType));
	}
}

void UInventoryComponent::NotifyIfPickedUpObjectIsAmmo(TScriptInterface<IPickupObject> pickableObject) const
{
	if(pickableObject.GetObject()->IsA<AAmmo>())
	{
		const auto ammo = Cast<AAmmo>(pickableObject.GetObject());
		OnPickedUpAmmoDelegate.Broadcast(ammo->GetAmmoType(), GetTotalAmmoOfType(ammo->GetAmmoType()));
	}
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
