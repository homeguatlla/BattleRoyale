// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IFootstepsComponent.generated.h"


// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIFootstepsComponent : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IIFootstepsComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IIFootstepsComponent")
	virtual void HandleFootstep(bool isRightFoot) const = 0;
};

 