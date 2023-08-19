// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPickupObjectState : uint8
{
	Initial UMETA(DisplayName = "Initial State"),
	Equipped UMETA(DisplayName = "Equipped State"),
	Dropped UMETA(DisplayName = "Dropped State"),
	InInventory UMETA(DisplayName = "In Inventory State"),
	Max UMETA(DisplayName = "Max State")
};