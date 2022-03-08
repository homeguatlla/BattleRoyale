// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICharacter.generated.h"

class IIAbilitySystemInterfaceBase;
class IAbilitySystemInterface;
class IIWeapon;
// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UICharacter : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IICharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual TScriptInterface<IIWeapon> GetEquippedWeapon() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsCharacterValid() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FVector GetCurrentVelocity() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FRotator GetCurrentRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FRotator GetCurrentControlRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual float GetCurrentHealth() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual float GetMaxHealth() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsFalling() const  = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool CanSprint() const  = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void StartSprinting() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void StopSprinting() = 0;

	/* CanJump is already defined in the Character.h so, we added _*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool CanJump() const  = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void StartJumping() = 0;

	/* StopJumping is already defined in the Character.h so, we added _*/
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void StopJumping_() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool CanShoot() const = 0;

	/**
	 * Shoot is aim and try to hit something
	 **/
	virtual void ServerShoot() = 0;

	virtual void Shoot() = 0;
	
	virtual UAnimMontage* GetShootingMontage() const = 0;

	virtual UAnimMontage* GetSimulatedShootingMontage() const = 0;

	virtual UAnimInstance* GetAnimationInstance() const = 0;

	virtual IAbilitySystemInterface* GetAbilitySystemComponent() const = 0;

	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentBase() const = 0;
};
 