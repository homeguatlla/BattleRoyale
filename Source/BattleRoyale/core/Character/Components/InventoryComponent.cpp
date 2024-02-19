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
		mInventoryBag->AddItem(item, itemDefault->GetValue(), itemDefault->GetValue());
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
			GEngine->AddOnScreenDebugMessage(
				-1,
				0,
				FColor::Blue,
				FString::Printf(TEXT("%s"), *inventoryItemStaticData->GetItemName().ToString())
				);
			return false;
		});
	}
}

bool UInventoryComponent::IsAWeapon(TScriptInterface<IPickupObject> pickableObject) const
{
	return pickableObject.GetObject()->Implements<UWeapon>();
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
		//This object is equipped by someone else
		return false;
	}
	
	//Save picked up object to the inventory.
	if(mInventoryBag->IsFull())
	{
		return false;	
	}

	if(IsAWeapon(pickableObject) && GetTotalWeapons() >= MaxInventoryWeapons)
	{
		return false;
	}

	if(const auto pickableObjectBase = Cast<APickableObjectBase>(pickableObject.GetObject()))
	{
		ClientNotifyPickedUpObject(pickableObjectBase);
	}
	
	if(!HasItemEquipped() && pickableObject->CanBeEquipped())
	{
		return EquipObject(pickableObject);
	}
	
	NotifyIfPickedUpObjectIsAmmo(pickableObject);

	//Add item into the inventory
	int value2 = 0;
	if(IsAWeapon(pickableObject))
	{
		const auto weapon = Cast<AWeaponBase>(pickableObject.GetObject());
		value2 = weapon->GetAmmo();
	}
	
	mInventoryBag->AddItem(pickableObject->GetInventoryItemStaticData(), pickableObject->GetValue(), value2);
	
	//Once the pickable object has been saved into the inventory we can remove it from the world.
	const auto pickableObjectActor = Cast<APickableObjectBase>(pickableObject.GetObject());
	if(pickableObjectActor)
	{
		pickableObjectActor->Destroy();
	}
	return true;
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

bool UInventoryComponent::UnEquipItem()
{
	if(!HasItemEquipped())
	{
		return false;	
	}
	const TScriptInterface<IPickupObject> pickableObject = GetEquippedItem();
	const auto object = Cast<APickableObjectBase>(pickableObject.GetObject());
	check(object);

	int value2 = 0;
	if(IsAWeapon(pickableObject))
	{
		const auto weapon = Cast<AWeaponBase>(pickableObject.GetObject());
		value2 = weapon->GetAmmo();
	}
	
	mInventoryBag->AddItem(pickableObject->GetInventoryItemStaticData(), pickableObject->GetValue(), value2);
	
	//TODO esto se podría meter en un método UnEquipObject
	pickableObject->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	pickableObject->OnUnEquipped();
	OnUnEquippedPickableObject.Broadcast();
	mEquippedObject = nullptr;
	object->Destroy();
	
	return true;
}

bool UInventoryComponent::EquipItem(UInventoryItemInstance* item)
{
	if(!item)
	{
		return false;
	}

	if(HasItemEquipped())
	{
		return false;
	}

	const auto staticData = item->GetStaticData();
	if(!staticData->CanBeEquipped())
	{
		return false;
	}

	const auto character = Cast<ACharacterBase>(GetOwner());
	if(const auto pickableObject = GetWorld()->SpawnActorDeferred<APickableObjectBase>(
		staticData->GetPickupObjectClass(),
		FTransform(),
		character,
		character,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
		))
	{
		if(IsAWeapon(pickableObject))
		{
			const auto weapon = Cast<AWeaponBase>(pickableObject);
			weapon->SetAmmo(item->GetValue2());
		}
		EquipObject(pickableObject);
		
		pickableObject->SetValue(item->GetValue1());
		mInventoryBag->RemoveItem(item);
		pickableObject->FinishSpawning(FTransform());
		return true;
	}
	return false;
}

bool UInventoryComponent::EquipObject(TScriptInterface<IPickupObject> pickableObject)
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
			//It is possible inventoryItem is null because the array could be replicated,
			//but maybe the items not.
			if(inventoryItem)
			{
				const auto staticData = inventoryItem->mInventoryItem->GetStaticData();
				const auto defaultPickableObject = staticData->GetPickableObject();
				if(defaultPickableObject->IsA<AAmmo>())
				{
					const auto ammo = Cast<AAmmo>(defaultPickableObject);
					if(ammo->GetAmmoType() == ammoType)
					{
						totalAmmo += inventoryItem->mInventoryItem->GetValue1();
					}
				}
			}
			return false;
		});

	return totalAmmo;
}

int UInventoryComponent::GetTotalWeapons() const
{
	int totalWeapons = 0;
	
	PerformActionForEachInventoryItem(
	[&totalWeapons](UInventoryArrayItem* inventoryItem) -> bool
	{
		check(inventoryItem);
		const auto pickableObject = inventoryItem->mInventoryItem->GetStaticData()->GetPickableObject();
		if(pickableObject->Implements<UWeapon>())
		{
			totalWeapons++;
		}
		return false;	
	});
	if(HasItemEquipped() && IsAWeapon(mEquippedObject))
	{
		totalWeapons++;
	}
	return totalWeapons;
}

UInventoryItemInstance* UInventoryComponent::GetNextWeaponDifferentThan(TScriptInterface<IPickupObject> weapon) const
{
	UInventoryItemInstance* nextWeapon = nullptr;
	
	PerformActionForEachInventoryItem(
	[&nextWeapon](UInventoryArrayItem* inventoryItem) -> bool
	{
		const auto pickableObject = inventoryItem->mInventoryItem->GetStaticData()->GetPickableObject();
		if(pickableObject->Implements<UWeapon>())
		{
			nextWeapon = inventoryItem->mInventoryItem;
			return true;
		}
		return false;
	});
	
	return nextWeapon;
}

int UInventoryComponent::RemoveEnoughAmmo(EAmmoType ammoType, int ammoNeeded)
{
	int ammoRemoved = 0;
	auto inventoryItemInstance = GetAmmoItemOfType(ammoType);

	while(ammoRemoved < ammoNeeded && inventoryItemInstance != nullptr)
	{
		const auto ammoAvailable = inventoryItemInstance->GetValue1();
		if(ammoRemoved + ammoAvailable >= ammoNeeded)
		{		
			const auto newAmmoValue = ammoAvailable - (ammoNeeded - ammoRemoved);
			if(newAmmoValue == 0)
			{
				mInventoryBag->RemoveFirstItem(inventoryItemInstance->GetStaticDataClass());
			}
			else
			{
				inventoryItemInstance->UpdateValue1(newAmmoValue);
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
			check(staticData);
			const auto pickableObject = staticData->GetPickableObject();
			check(pickableObject);
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
		//TODO hay que pasar el equiped object para que el visual sepa pintarlo porque no está en el inventario
		gameInstance->GetEventDispatcher()->OnShowInventoryScreen.Broadcast(mInventoryBag, mEquippedObject);
	}
	mIsInventoryShown = !mIsInventoryShown;
}
