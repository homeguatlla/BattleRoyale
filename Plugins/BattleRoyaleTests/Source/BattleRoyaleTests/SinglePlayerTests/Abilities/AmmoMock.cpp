// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoMock.h"

#include "BattleRoyale/core/Utils/Inventory/InventoryItemStaticData.h"


// Sets default values
AAmmoMock::AAmmoMock()
{
	
}

void AAmmoMock::Initialize(const FName& itemStaticDataName, EPickupObjectState state, int value)
{
	mItemStaticData = LoadClass<UInventoryItemStaticData>(nullptr, *itemStaticDataName.ToString(), nullptr, LOAD_None, nullptr);
	mState = state;
	mValue = value;
}

