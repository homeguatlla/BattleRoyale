// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IItemSlot.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIItemSlot : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIItemSlot
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
	void SetEquipped(bool equipped);
};
