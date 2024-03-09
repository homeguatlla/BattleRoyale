// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInventoryBag.generated.h"


class UInventoryArrayItem;
class UInventoryItemStaticData;
class UInventoryItemInstance;
class IIInventoryItemInstance;
struct FInventoryArrayItem;

// This class does not need to be modified.
UINTERFACE()
class BATTLEROYALE_API UIInventoryBag : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIInventoryBag
{
	GENERATED_BODY()

public:
	virtual TScriptInterface<IIInventoryItemInstance> AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value, int value2) = 0;
	virtual void RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) = 0;
	virtual void RemoveItem(TScriptInterface<IIInventoryItemInstance> item) = 0;
	virtual void RemoveItemByID(int ID) = 0;
	
	virtual bool ExistItemWithID(int ID) const = 0;

	virtual void SetMaxItems(int max) = 0;
	virtual TScriptInterface<IIInventoryItemInstance> FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) = 0;
	virtual TScriptInterface<IIInventoryItemInstance> FindItemWithID(int ID) = 0;
	
	virtual void PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const = 0;
	virtual int Num() const = 0;
	virtual bool IsFull() const = 0;
	virtual bool IsEmpty() const = 0;
};
