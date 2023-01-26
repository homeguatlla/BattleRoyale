// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IWeapon.h"
#include "BattleRoyale/core/General/PickupObjectBase.h"
#include "WeaponBase.generated.h"

class UGameplayEffect;
class IICharacter;
UCLASS(Blueprintable)
class BATTLEROYALE_API AWeaponBase : public APickupObjectBase, public IWeapon
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AProjectileBase> ProjectileClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UTexture2D* CrossHair;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DistanceFromMuzzleLocation = { 20.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float CooldownTime = {3.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FGameplayTagContainer CooldownTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName LeftHandSocketName;
	
	//This is where the left hand goes on the weapon.
	FTransform mLeftHandSocketTransform;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> MuzzleGameplayEffect;
	
public:	
	AWeaponBase();
	
	virtual FVector GetMuzzleLocation() const override;
	virtual FRotator GetMuzzleRotation() const override;
	virtual float GetCooldownTime() const override { return CooldownTime; }
	virtual FGameplayTagContainer GetCooldownTags() const override { return CooldownTags; }
	virtual FTransform GetLeftHandSocketTransform() override;
	virtual FTransform SaveLeftHandSocketTransform() override;
	virtual void Destroy() override;
	virtual bool CanBeFired() const override;
	virtual void Fire(const FVector& targetLocation) override;
	
	virtual void SetCharacterOwner(ACharacterBase* character) override;
	virtual void SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation) override;
	virtual UTexture2D* GetCrossHairTexture() const override { return CrossHair; }
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (DisplayName = OnFire))
	void BP_OnFire();

private:
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector& muzzleLocation, const FVector& targetLocation) const;
	
	void SpawnProjectileServer(const FVector& muzzleLocation, const FVector& shootingDirection) const;
	FVector GetProjectileSpawnLocation(const FVector& location, const FVector& direction, float distanceFromMuzzleLocation) const;
};
