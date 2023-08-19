// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInventoryComponent.generated.h"

class IPickupObject;

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
};
