// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInventoryBag.h"
#include "InventoryArray.h"
#include "BattleRoyale/core/Utils/NetworkObject.h"
#include "UObject/Object.h"
#include "InventoryBag.generated.h"

UCLASS()
class BATTLEROYALE_API UInventoryBag : public UNetworkObject, public IIInventoryBag
{
	GENERATED_BODY()
	
	//virtual bool IsNameStableForNetworking() const override { return true; }
	
public:
	UInventoryBag();

	virtual TScriptInterface<IIInventoryItemInstance> AddItem(TSubclassOf<UInventoryItemStaticData> itemClass, int value, int value2) override;
	virtual void RemoveFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	virtual void RemoveItem(TScriptInterface<IIInventoryItemInstance> item) override;
	
	virtual void SetMaxItems(int max) override { mMaxItems = max; }
	virtual TScriptInterface<IIInventoryItemInstance> FindFirstItem(TSubclassOf<UInventoryItemStaticData> itemClass) override;
	
	virtual bool ExistItemWithID(int ID) const override;
	virtual TScriptInterface<IIInventoryItemInstance> FindItemWithID(int ID) override;
	
	virtual void PerformActionForEachItem(const std::function<bool(UInventoryArrayItem* inventoryItem)>& action) const override;
	virtual int Num() const override { return mInventoryArray->Num(); }
	virtual bool IsFull() const override { return mInventoryArray->Num() >= mMaxItems; }
	virtual bool IsEmpty() const override { return mInventoryArray->Num() <= 0; }
	TSubclassOf<UUserWidget> GetItemWidgetClassByIndex(int index) const;

	void AddReplicatedSubObject(UActorComponent* owner);

	//UNetworkObject override
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	UPROPERTY(Replicated)
	UInventoryArray* mInventoryArray;
	
	int mMaxItems = 5;
};
