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

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual bool IsAutomaticFireEnabled() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual float GetAutomaticFireDelay() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual int32 GetAmmo() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual int32 GetMagazineCapacity() const = 0;

	/**
	 * Fire is to activate the mechanism of the weapon. Pulling the trigger.
	 **/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual void Fire(const FVector& shootingDirection) = 0;

	//This method will be called from Blueprint (Gameplay cue shoot that makes the visual effects of the weapon when firing)
	UFUNCTION(BlueprintCallable, Category="Weapon")
	virtual void OnFire() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual TScriptInterface<ICrosshair> GetCrosshairWidget() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FTransform GetLeftHandSocketTransform() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FTransform GetMuzzleSocketTransform() = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual FTransform GetCrosshairSocketTransform() = 0;
	
	virtual FTransform SaveLeftHandSocketTransform() = 0;
	virtual void SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation) = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IWeapon")
	virtual UAnimationAsset* GetShootingAnimation() const = 0;


	virtual void StartAiming(const FVector& location, const FRotator& rotation) = 0;
	virtual void StopAiming() = 0;

	virtual FVector GetForwardVector() const = 0;
	
	/**
	 * Aiming and FOV 
	 */

	virtual float GetZoomedFOV() const = 0;
	virtual float GetZoomInterpolationSpeed() const = 0;
};
