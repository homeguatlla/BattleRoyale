﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemStaticData.generated.h"


class APickupObjectBase;

UCLASS(Blueprintable, BlueprintType)
class BATTLEROYALE_API UInventoryItemStaticData : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemStaticData(){}
	UInventoryItemStaticData(const FName& name, TSubclassOf<APickupObjectBase> pickableObjectClass);
	FName GetItemName() const { return mName; }
	TSubclassOf<APickupObjectBase> GetPickupObjectClass() const { return mPickupObjectClass;}
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Name"))
	FName mName;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Pickup Object Class (Visual)"))
	TSubclassOf<APickupObjectBase> mPickupObjectClass;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Can be equipped"))
	bool mCanBeEquipped = true;
};
