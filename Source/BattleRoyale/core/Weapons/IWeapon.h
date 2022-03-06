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
	//UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	//virtual USkeletalMeshComponent* GetMesh() const = 0;

	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName) = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FVector GetMuzzleLocation() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FRotator GetMuzzleRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual bool CanBeFired() const = 0;

	/**
	 * Fire is to activate the mechanism of the weapon. Pulling the trigger.
	 **/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual void Fire() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual void FireClient(bool isFirstPerson) = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual USoundBase* GetFireSound() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual UTexture2D* GetCrossHairTexture() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual UParticleSystem* GetMuzzleEffect() const = 0;
};