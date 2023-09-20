// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Utils/Inventory/IInventoryBag.h"
#include "BattleRoyale/core/Utils/Inventory/IInventoryItemInstance.h"
#include "UObject/Object.h"
#include "InventoryBagMock.generated.h"

struct BATTLEROYALETESTS_API InventoryItemMock : public IIInventoryItemInstance
{
	InventoryItemMock(int _id, int _type, int _value = 0, UInventoryItemStaticData* _data = nullptr) : id(_id), type(_type), value(_value), data(_data) {}
	virtual void Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int _value) override {}
	virtual const UInventoryItemStaticData* GetStaticData() const override { return data; }
	virtual int GetValue() const override { return value; }
	virtual void OnEquipped() override {}
	virtual void OnUnEquipped() override {}
	
	int id;
	int type;
	int value;
	UInventoryItemStaticData* data = nullptr;
};
/**
 * 
 */
UCLASS()
class BATTLEROYALETESTS_API UInventoryBagMock : public UObject, public IIInventoryBag
{
	GENERATED_BODY()

public:
	void Initialize(int maxItems, int numItems) { mMaxItems = maxItems; mNumItems = numItems; }
	void AddItem(const InventoryItemMock& item) { mItems.Add(item); }
	void RemoveItem(int index) { if(index >= 0 && index < mItems.Num()) mItems.RemoveAt(index); }

	virtual void AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value) override;
	virtual void RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	virtual bool ExistItemWithID(int ID) const override;
	virtual void SetMaxItems(int max) override;
	virtual TScriptInterface<IIInventoryItemInstance> FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	virtual void PerformActionForEachItem(const std::function<bool(const FInventoryArrayItem& inventoryItem)>& action) override;
	virtual int Num() const override;
	virtual bool IsFull() const override;
	virtual bool IsEmpty() const override;

private:
	int mMaxItems = 0;
	int mNumItems = 0;
	TArray<InventoryItemMock> mItems;
};
