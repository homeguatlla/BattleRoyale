// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Interface.h"
#include "IWeapon.generated.h"

class ACharacterBase;
// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UWeapon : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IWeapon
{
	GENERATED_BODY()

public:
	//UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	//virtual USkeletalMeshComponent* GetMesh() const = 0;

	virtual void Destroy() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FVector GetMuzzleLocation() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FRotator GetMuzzleRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual float GetCooldownTime() const = 0;
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FGameplayTagContainer GetCooldownTags() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual bool CanBeFired() const = 0;

	/**
	 * Fire is to activate the mechanism of the weapon. Pulling the trigger.
	 **/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual void Fire() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual USoundBase* GetFireSound() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual UTexture2D* GetCrossHairTexture() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual UParticleSystem* GetMuzzleEffect() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FTransform GetLeftHandSocketTransform() = 0;
	
	virtual void SetCharacterOwner(ACharacterBase* character) = 0;

	virtual FTransform SaveLeftHandSocketTransform() = 0;
	virtual void SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation) = 0;
};