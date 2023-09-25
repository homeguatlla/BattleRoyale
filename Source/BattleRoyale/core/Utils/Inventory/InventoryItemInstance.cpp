// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "BattleRoyale/core/Utils/GameplayBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int value)
{
	mStaticDataClass = itemStaticClass;
	mValue = value;
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
	DOREPLIFETIME(UInventoryItemInstance, mValue);
}

void UInventoryItemInstance::OnRep_IsEquipped()
{
}

void UInventoryItemInstance::UpdateValue(int newValue)
{
	mValue = newValue;
}

void UInventoryItemInstance::OnEquipped()
{
	mIsEquipped = true;
}

void UInventoryItemInstance::OnUnEquipped()
{
	mIsEquipped = false;
}
