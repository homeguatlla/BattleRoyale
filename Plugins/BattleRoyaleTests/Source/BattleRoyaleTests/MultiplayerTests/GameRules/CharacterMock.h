// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "GameFramework/Character.h"
#include "CharacterMock.generated.h"

UCLASS()
class BATTLEROYALETESTS_API ACharacterMock : public ACharacter, public IICharacter
{
	GENERATED_BODY()

public:
	ACharacterMock();

	virtual float GetCurrentHealth() const override { return mHealth; }
	virtual void SetCurrentHealthTest(float health) override { mHealth = health; }
	virtual bool IsAlive() const override { return mHealth > 0; }


	virtual FVector GetLocation() const override { return FVector::Zero(); }
	virtual bool IsCharacterValid() const override { return true; }
	virtual FVector GetCurrentVelocity() const override { return FVector::Zero(); }
	virtual FVector GetCurrentMeshSpaceVelocity() const override { return FVector::Zero(); }
	virtual FRotator GetCurrentRotation() const override { return FRotator(); }
	virtual FRotator GetCurrentControlRotation() const override { return FRotator(); }
	virtual void SetInvulnerableServer(bool isInvulnerable) override {}
	virtual float GetMaxHealth() const override { return 100.0f; }
	virtual float GetMaxWalkSpeed() const override { return 100.0f; }
	virtual float GetMinWalkSpeed() const override { return 10.0f; }
	virtual void SetMaxSpeed(float maxSpeed) override {}
	virtual bool IsFalling() const override { return false;}
	virtual bool CanSprint() const override { return false; }
	virtual void StartSprinting() override {}
	virtual void StopSprinting() override {}
	virtual bool CanJump() const override { return false; }
	virtual void StartJumping() override {}
	virtual void StopJumping_() override {}
	virtual bool CanCrouch_() const override { return false;}
	virtual void StartCrouching() override{}
	virtual void StopCrouching() override{}
	virtual bool IsCrouching() const override { return false;}
	virtual bool CanShoot() const override { return false;}
	virtual void DieServer() override{}
	virtual void DieClient() override{}
	virtual void SetEnableInput(bool enable, const FInputModeDataBase& inputMode) override{}
	virtual UCameraComponent* GetCamera() const override { return nullptr;}
	virtual UHurtComponent* GetHurtComponent() const override { return nullptr;}
	virtual void NotifyRefreshHealth(float health) const override{}
	virtual void NotifyTakeDamage(float damage, const AActor* causer, float currentHealth) override{}
	virtual FName GetRightHandSocketName() const override{return FName();}
	virtual FTransform GetRightHandSocketTransform() const override{ return FTransform();}
	virtual FName GetRightFootSocketName() const override{return FName();}
	virtual FTransform GetRightFootSocketTransform() const override{ return FTransform();}
	virtual FName GetLeftFootSocketName() const override{return FName();}
	virtual FTransform GetLeftFootSocketTransform() const override{ return FTransform();}
	virtual void Shoot() override{}
	virtual TScriptInterface<IGunComponent> GetGunComponent() const override{ return nullptr;}
	virtual IAbilitySystemInterface* GetAbilitySystemComponent() const override{return nullptr;}
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentBase() const override{return nullptr;}
	virtual TScriptInterface<IIFootstepsComponent> GetFootstepsComponent() const override {return nullptr;}
	virtual TScriptInterface<IIInventoryComponent> GetInventoryComponent() const override {return nullptr;}
	virtual UPickupSelectorComponent* GetPickupSelectorComponent() const override { return nullptr; }
private:
	float mHealth {100};
};
