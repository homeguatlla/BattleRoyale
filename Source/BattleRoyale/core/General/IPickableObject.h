// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPickableObject.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UPickableObject : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IPickableObject
{
	GENERATED_BODY()

public:
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName) = 0;
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) = 0;
	
	virtual FVector GetPickupWidgetLocation(const FBoxSphereBounds& bounds) const = 0;
};