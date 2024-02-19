﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "BattleRoyale/core/PickableObjects/Ammo/AmmoTypes.h"
#include "BattleRoyale/core/PickableObjects/Weapons/IWeapon.h"
#include "WeaponMock.generated.h"

UCLASS()
class BATTLEROYALETESTS_API AWeaponMock : public AActor, public IWeapon, public IPickupObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AWeaponMock();

	void Initialize(EPickupObjectState state, int ammo);
	
	virtual TSubclassOf<UInventoryItemStaticData> GetInventoryItemStaticData() const override { return nullptr; }
	virtual EPickupObjectState GetState() const override { return mState; }
	virtual int GetValue() const override { return mValue; }
	virtual FVector GetLocation() const override { return FVector::Zero(); }
	virtual FName GetPickableName() const override { return FName("");}
	virtual void SetValue(int value) override {}
	virtual void SetCharacterOwner(ACharacterBase* character) override {}
	virtual void OnEquipped() override {}
	virtual void OnUnEquipped() override {}
	virtual void OnDropped() override {}
	virtual bool IsEquipped() const override { return mState == EPickupObjectState::Equipped; }
	virtual bool CanBeEquipped() const override { return true; }
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent,
	const FAttachmentTransformRules& attachmentRules, const FName& socketName) override { return true;}
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) override {}
	virtual FVector GetMuzzleLocation() const override { return FVector::Zero(); }
	virtual FRotator GetMuzzleRotation() const override { return FRotator::ZeroRotator; }
	virtual float GetCooldownTime() const override { return 0.0f; }
	virtual FGameplayTagContainer GetCooldownTags() const override { return FGameplayTagContainer(); }
	virtual bool CanBeFired() const override { return HasAmmo(); }
	virtual bool IsAutomaticFireEnabled() const override { return false; }
	virtual float GetAutomaticFireDelay() const override { return 0.0f; }
	virtual int32 GetAmmo() const override { return mAmmo; }
	virtual void SetAmmo(int32 newAmmo) override {}
	virtual int32 GetMagazineCapacity() const override { return 10;}
	virtual EAmmoType GetAmmoType() const override { return EAmmoType::LightAmmo; }
	virtual bool IsMagazineFull() const override { return false; }
	virtual bool HasAmmo() const override { return mAmmo>0; }
	virtual void Reload(int ammoAmount) override {}
	virtual void Fire(const FVector& shootingDirection) override {}
	virtual void OnFire() override {}
	virtual TScriptInterface<ICrosshair> GetCrosshairWidget() const override { return nullptr; }
	virtual FTransform GetLeftHandSocketTransform() override { return FTransform(); }
	virtual FTransform GetMuzzleSocketTransform() override { return FTransform(); }
	virtual FTransform GetCrosshairSocketTransform() override { return FTransform(); }
	virtual FTransform SaveLeftHandSocketTransform() override { return FTransform(); }
	virtual void SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation) override {}
	virtual UAnimationAsset* GetShootingAnimation() const override { return nullptr; }
	virtual void StartAiming(const FVector& location, const FRotator& rotation) override {}
	virtual void StopAiming() override {}
	virtual FVector GetForwardVector() const override { return FVector::Zero(); }
	virtual float GetZoomedFOV() const override { return 0.0f; }
	virtual float GetZoomInterpolationSpeed() const override { return 0.0f; }

private:
	EPickupObjectState mState;
	int mAmmo;
	int mValue;
};