// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGunComponent.generated.h"

class IWeapon;
class ACharacterBase;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UGunComponent : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IGunComponent
{
	GENERATED_BODY()

public:
	virtual bool UnEquipWeapon() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual TScriptInterface<IWeapon> GetEquippedWeapon() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual bool HasWeaponEquipped() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual bool CanAim() const  = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual void StartAiming() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual void StopAiming() = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual bool IsAiming() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual bool CanShoot() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual FVector GetShootingTargetLocation() const = 0;
	
	/**
	 * Shoot is aim and try to hit something
	 **/

	virtual void Shoot() = 0;
	virtual void ReleaseTrigger() = 0;
	virtual UAnimMontage* GetShootingMontage() const = 0;
	virtual UAnimMontage* GetReloadingMontage() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGunComponent")
	virtual bool CanReload(const TScriptInterface<IIInventoryComponent> inventoryComponent) const = 0;
	virtual void Reload(const TScriptInterface<IIInventoryComponent> inventoryComponent) = 0;
	
	virtual void SetupLeftHandSocketTransform(const ACharacterBase* character) const = 0;
};

 