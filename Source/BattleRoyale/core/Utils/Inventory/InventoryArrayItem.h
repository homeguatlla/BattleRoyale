// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InventoryItemInstance.h"
#include "BattleRoyale/core/Utils/GameplayBlueprintFunctionLibrary.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "InventoryArrayItem.generated.h"

class UInventoryItemStaticData;
class UInventoryItemInstance;

USTRUCT(BlueprintType)
struct FInventoryArrayItem : public FFastArraySerializerItem
{
	GENERATED_BODY()
	
	UPROPERTY()
	UInventoryItemInstance* mInventoryItem;
};
