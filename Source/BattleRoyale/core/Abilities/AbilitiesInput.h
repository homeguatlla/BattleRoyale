
#pragma once

#include "CoreMinimal.h"

UENUM(BlueprintType)
enum class EAbilityInputID : uint8
{
	None,
	Confirm,
	Cancel,
	Fire,
	Aim,
	Reload,
	SwapWeapons,
	Inventory
};