// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IInventoryItemOverlay.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UIInventoryItemOverlay : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIInventoryItemOverlay
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
	void SetValue(int value);
};
