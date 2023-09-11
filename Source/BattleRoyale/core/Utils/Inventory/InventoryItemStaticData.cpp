﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemStaticData.h"

UInventoryItemStaticData::UInventoryItemStaticData(const FName& name,
	TSubclassOf<APickupObjectBase> pickableObjectClass) :
mName(name),
mPickupObjectClass(pickableObjectClass)
{
}