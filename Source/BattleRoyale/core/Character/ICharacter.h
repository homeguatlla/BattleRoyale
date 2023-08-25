// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "UObject/Interface.h"
#include "ICharacter.generated.h"

class UPickupComponent;
class IIInventoryComponent;
class IPickupObject;
class IIAbilitySystemInterfaceBase;
class IAbilitySystemInterface;
class IWeapon;
class UCameraComponent;
class UHurtComponent;
class IGunComponent;
class IIFootstepsComponent;
class IICharacter;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class BATTLEROYALE_API UICharacter : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IICharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FVector GetLocation() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsCharacterValid() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FVector GetCurrentVelocity() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FVector GetCurrentMeshSpaceVelocity() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FRotator GetCurrentRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FRotator GetCurrentControlRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual float GetCurrentHealth() const = 0;

	virtual void SetCurrentHealthTest(float health) = 0;

	virtual void SetInvulnerableServer(bool isInvulnerable) = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual float GetMaxHealth() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual float GetMaxWalkSpeed() const = 0;

	UFUNCTION(BlueprintCallable)
	virtual float GetMinWalkSpeed() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsAlive() const = 0;
	
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
	virtual bool CanCrouch_() const  = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void StartCrouching() = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void StopCrouching() = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsCrouching() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool CanShoot() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void DieServer() = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual void DieClient() = 0;
	
	virtual void SetEnableInput(bool enable, const FInputModeDataBase& inputMode = FInputModeGameAndUI()) = 0;

	virtual UCameraComponent* GetCamera() const = 0;

	virtual UHurtComponent* GetHurtComponent() const = 0;

	virtual void NotifyRefreshHealth(float health) const = 0;
	
	virtual void NotifyTakeDamage(float damage, const AActor* causer, float currentHealth) = 0;
	
	/**
	 * Shoot is aim and try to hit something
	 **/

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FName GetRightHandSocketName() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FTransform GetRightHandSocketTransform() const = 0;
	
	virtual void Shoot() = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FName GetRightFootSocketName() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FTransform GetRightFootSocketTransform() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FName GetLeftFootSocketName() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FTransform GetLeftFootSocketTransform() const = 0;
	
	UFUNCTION(BlueprintCallable, Category = "ICharacter")
	virtual TScriptInterface<IGunComponent> GetGunComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "ICharacter")
	virtual TScriptInterface<IIFootstepsComponent> GetFootstepsComponent() const = 0;

	UFUNCTION(BlueprintCallable, Category = "ICharacter")
	virtual TScriptInterface<IIInventoryComponent> GetInventoryComponent() const = 0;
	
	virtual IAbilitySystemInterface* GetAbilitySystemComponent() const = 0;

	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentBase() const = 0;
};
 