// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGunComponent.generated.h"

class IWeapon;

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
	virtual bool EquipWeapon(TScriptInterface<IWeapon> weapon, const FName& socketName) = 0;
	virtual bool UnEquipWeapon() const = 0;
	
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
	
	/**
	 * Shoot is aim and try to hit something
	 **/

	virtual void Shoot() const = 0;	
	virtual UAnimMontage* GetShootingMontage() const = 0;
};
 