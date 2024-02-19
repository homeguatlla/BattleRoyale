// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemStaticData.h"
#include "BattleRoyale/core/PickableObjects/PickableObjectBase.h"

UInventoryItemStaticData::UInventoryItemStaticData(const FName& name,
	TSubclassOf<APickableObjectBase> pickableObjectClass) :
mName(name),
mPickupObjectClass(pickableObjectClass)
{
}

APickableObjectBase* UInventoryItemStaticData::GetPickableObject() const
{
	const auto objectClass = GetPickupObjectClass();
	return objectClass.GetDefaultObject();
}
