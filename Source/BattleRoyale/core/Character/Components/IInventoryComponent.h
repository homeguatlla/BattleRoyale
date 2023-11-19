// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "BattleRoyale/core/PickableObjects/Ammo/AmmoTypes.h"
#include "IInventoryComponent.generated.h"

class IPickupObject;
class UInventoryItemStaticData;
class UInventoryArrayItem;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIInventoryComponent : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIInventoryComponent
{
	GENERATED_BODY()

public:
	virtual bool PickupObjectServer(TScriptInterface<IPickupObject> pickableObject) = 0;
	virtual bool EquipItem(TScriptInterface<IPickupObject> pickableObject) = 0;
	virtual bool DropObjectServer() = 0;
	virtual TScriptInterface<IPickupObject> GetEquippedItem() const = 0;
	virtual bool HasItemEquipped() const = 0;

	virtual bool HasItemOfType(TSubclassOf<UInventoryItemStaticData> itemStaticDataClassToFind) const = 0;
	virtual bool HasAmmoOfType(EAmmoType ammoType) const = 0;
	virtual bool HasLifeKid() const = 0;
	
	virtual int RemoveEnoughAmmo(EAmmoType ammoType, int ammoNeeded) = 0;
	
	virtual void PerformActionForEachInventoryItem(const std::function<bool (UInventoryArrayItem* inventoryItem)>& callback) const = 0;
};
