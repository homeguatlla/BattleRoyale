
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Sprint,
	Fire,
	PickUp,
	Drop,
	Aim,
	Reload,
	SwapWeapons,
	Inventory
};