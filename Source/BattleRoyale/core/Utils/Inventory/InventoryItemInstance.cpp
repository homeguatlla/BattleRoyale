// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemInstance.h"

#include "BattleRoyale/core/Utils/GameplayBlueprintFunctionLibrary.h"
#include "Net/UnrealNetwork.h"

void UInventoryItemInstance::Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int value1, int value2)
{
	mStaticDataClass = itemStaticClass;
	mValue1 = value1;
	mValue2 = value2;
}

const UInventoryItemStaticData* UInventoryItemInstance::GetStaticData() const
{
	return UGameplayBlueprintFunctionLibrary::GetInventoryItemStaticData(mStaticDataClass);
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UInventoryItemInstance, mStaticDataClass);
	DOREPLIFETIME(UInventoryItemInstance, mValue1);
	DOREPLIFETIME(UInventoryItemInstance, mValue2);
}


void UInventoryItemInstance::UpdateValue1(int newValue)
{
	mValue1 = newValue;
}

void UInventoryItemInstance::UpdateValue2(int newValue)
{
	mValue2 = newValue;
}
