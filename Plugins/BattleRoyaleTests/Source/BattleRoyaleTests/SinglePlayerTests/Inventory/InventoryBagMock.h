// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Utils/Inventory/IInventoryBag.h"
#include "BattleRoyale/core/Utils/Inventory/IInventoryItemInstance.h"
#include "UObject/Object.h"
#include "InventoryBagMock.generated.h"

struct BATTLEROYALETESTS_API InventoryItemMock : public IIInventoryItemInstance
{
	InventoryItemMock(int _id, int _type, int _value1 = 0, int _value2 = 0, UInventoryItemStaticData* _data = nullptr) : mId(_id), mType(_type), mValue1(_value1), mValue2(_value2), mData(_data) {}
	virtual void Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int value1, int value2) override {}
	virtual const UInventoryItemStaticData* GetStaticData() const override { return mData; }
	virtual const TSubclassOf<UInventoryItemStaticData> GetStaticDataClass() const override { return nullptr; }
	virtual int GetValue1() const override { return mValue1; }
	virtual void UpdateValue1(int newValue) override { mValue1 = newValue; }
	virtual int GetValue2() const override { return mValue2; }
	virtual void UpdateValue2(int newValue) override { mValue2 = newValue; }
	
	int mId;
	int mType;
	int mValue1;
	int mValue2;
	UInventoryItemStaticData* mData = nullptr;
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
	TScriptInterface<IIInventoryItemInstance> AddItem(const InventoryItemMock& item) { mItems.Add(item); return nullptr;}
	void RemoveItem(int index) { if(index >= 0 && index < mItems.Num()) mItems.RemoveAt(index); }

	virtual TScriptInterface<IIInventoryItemInstance> AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value1, int value2) override;
	virtual void RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	virtual void RemoveItem(TScriptInterface<IIInventoryItemInstance> item) override;
	virtual void RemoveItemByID(int ID) override;
	virtual bool ExistItemWithID(int ID) const override;
	virtual void SetMaxItems(int max) override;
	virtual TScriptInterface<IIInventoryItemInstance> FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	virtual TScriptInterface<IIInventoryItemInstance> FindItemWithID(int ID) override;
	virtual void PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const override;
	virtual int Num() const override;
	virtual bool IsFull() const override;
	virtual bool IsEmpty() const override;

private:
	int mMaxItems = 0;
	int mNumItems = 0;
	TArray<InventoryItemMock> mItems;
};
