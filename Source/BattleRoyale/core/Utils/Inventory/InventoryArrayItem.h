// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Utils/NetworkObject.h"
#include "InventoryItemInstance.h"
#include "InventoryArrayItem.generated.h"

class UInventoryItemStaticData;
class IIInventoryItemInstance;

UCLASS()
class UInventoryArrayItem : public UNetworkObject //public FFastArraySerializerItem
{
	GENERATED_BODY()

public:
	UInventoryArrayItem();

	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	UInventoryItemInstance* mInventoryItem = nullptr;
	//TScriptInterface<IIInventoryItemInstance> mInventoryItem;
	
	int GetID() const { return mID; }

	//Test purposes
	void SetID(int id) { mID = id; }
private:
	static int Counter;

	UPROPERTY(Replicated)
	int mID = 0;
};
