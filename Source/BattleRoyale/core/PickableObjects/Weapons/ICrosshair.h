// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICrosshair.generated.h"

UINTERFACE()
class UCrosshair : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API ICrosshair
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Crosshair Interface")
	void OnRefreshCrosshair(float spread, AActor* targetActor, bool isAiming, bool isMagazineAlmostEmpty);
};