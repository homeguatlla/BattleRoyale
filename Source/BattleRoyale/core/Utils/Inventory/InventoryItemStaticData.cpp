// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemStaticData.h"

UInventoryItemStaticData::UInventoryItemStaticData(const FName& name,
	TSubclassOf<APickableObjectBase> pickableObjectClass) :
mName(name),
mPickupObjectClass(pickableObjectClass)
{
}
