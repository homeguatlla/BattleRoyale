// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IWeapon.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIWeapon : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IIWeapon
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual USkeletalMeshComponent* GetMesh() const = 0;
};