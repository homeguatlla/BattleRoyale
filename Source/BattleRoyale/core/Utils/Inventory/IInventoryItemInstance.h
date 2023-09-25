// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInventoryItemInstance.generated.h"

class UInventoryItemStaticData;

// This class does not need to be modified.
UINTERFACE()
class BATTLEROYALE_API UIInventoryItemInstance : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIInventoryItemInstance
{
	GENERATED_BODY()

public:
	virtual void Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int value) = 0;
	virtual const UInventoryItemStaticData* GetStaticData() const = 0;
	virtual const TSubclassOf<UInventoryItemStaticData> GetStaticDataClass() const = 0;	
	virtual int GetValue() const = 0;
	virtual void UpdateValue(int newValue) = 0;
	virtual void OnEquipped() = 0;
	virtual void OnUnEquipped() = 0;
};
