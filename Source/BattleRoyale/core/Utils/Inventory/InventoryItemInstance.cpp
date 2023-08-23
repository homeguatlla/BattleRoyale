// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "BattleRoyale/core/General/IPickupObject.h"
#include "BattleRoyale/core/Utils/GameplayBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass)
{
	mStaticDataClass = itemStaticClass;
}

const UInventoryItemStaticData* UInventoryItemInstance::GetStaticData() const
{
	return UGameplayBlueprintFunctionLibrary::GetInventoryItemStaticData(mStaticDataClass);
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, mStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, mIsEquipped);
}

void UInventoryItemInstance::OnRep_IsEquipped()
{
}

void UInventoryItemInstance::OnEquipped()
{
}

void UInventoryItemInstance::OnUnEquipped()
{
}
