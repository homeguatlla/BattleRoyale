// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemInstance.h"
#include "BattleRoyale/core/Utils/GameplayBlueprintFunctionLibrary.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryArrayItem.generated.h"

class UInventoryItemStaticData;
class IIInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryArrayItem : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FInventoryArrayItem()
	{
		mID = ++Counter;
	}
	
	UPROPERTY()
	TScriptInterface<IIInventoryItemInstance> mInventoryItem;

	int GetID() const { return mID; }

	//Test purposes
	void SetID(int id) { mID = id; }
private:
	static int Counter;
	
	int mID = 0;
};

int FInventoryArrayItem::Counter = 0;
