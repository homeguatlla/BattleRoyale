// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PickupObjectTypes.h"
#include "UObject/Interface.h"
#include "IPickupObject.generated.h"

class ACharacterBase;
class UInventoryItemStaticData;

// This class does not need to be modified.
UINTERFACE(BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class BATTLEROYALE_API UPickupObject : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IPickupObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual TSubclassOf<UInventoryItemStaticData> GetInventoryItemStaticData() const = 0;
	
	virtual EPickupObjectState GetState() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual int GetValue() const = 0;

	virtual FVector GetLocation() const = 0;
	virtual FName GetPickableName() const = 0;
	virtual void SetValue(int value) = 0;
	virtual void SetCharacterOwner(ACharacterBase* character) = 0;
	virtual void OnEquipped() = 0;
	virtual void OnUnEquipped() = 0;
	virtual void OnDropped() = 0;
	virtual bool IsEquipped() const = 0;
	virtual bool CanBeEquipped() const = 0;
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName) = 0;
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) = 0;
};