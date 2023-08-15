// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EPickupObjectState : uint8
{
	Initial UMETA(DisplayName = "Initial State"),
	PickedUp UMETA(DisplayName = "PickedUp State"),
	Dropped UMETA(DisplayName = "Dropped State"),
	Max UMETA(DisplayName = "Max State")
};