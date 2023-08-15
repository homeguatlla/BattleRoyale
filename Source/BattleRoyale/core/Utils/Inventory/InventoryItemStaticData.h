﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemStaticData.generated.h"


UCLASS(Blueprintable, BlueprintType)
class BATTLEROYALE_API UInventoryItemStaticData : public UObject
{
	GENERATED_BODY()

public:
	FName GetItemName() const { return mName; }
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Name"))
	FName mName;
};
