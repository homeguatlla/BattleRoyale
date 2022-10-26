// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPickupObject.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UPickupObject : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IPickupObject
{
	GENERATED_BODY()

public:
	virtual FVector GetPickupWidgetLocation(const FBoxSphereBounds& bounds) const = 0;
};