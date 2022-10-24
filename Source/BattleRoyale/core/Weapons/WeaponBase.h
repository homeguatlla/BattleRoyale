// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffectTypes.h"
#include "IWeapon.h"
#include "WeaponTypes.h"
#include "BattleRoyale/core/General/PickableObjectBase.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

class UGameplayEffect;
class IICharacter;
UCLASS(Blueprintable)
class BATTLEROYALE_API AWeaponBase : public APickableObjectBase, public IIWeapon
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AProjectileBase> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	UTexture2D* CrossHair;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DistanceFromMuzzleLocation = { 20.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float CooldownTime = {3.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FGameplayTagContainer CooldownTags;

	//UPROPERTY(EditAnywhere, Category = "Weapon")
	//TSubclassOf<UGameplayEffect> PickupIndicatorEffect;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool IsDebugEnabled { false };

	UPROPERTY(VisibleAnywhere, Category = "Weapon")
	EWeaponState WeaponState = EWeaponState::Initial;
	
public:	
	AWeaponBase();
	
	virtual void Tick( float DeltaSeconds ) override;
	
	//virtual USkeletalMeshComponent* GetMesh() const override { return Mesh; }

	virtual void SetWeaponState(EWeaponState state) override { WeaponState = state; }
	
	virtual FVector GetMuzzleLocation() const override;
	virtual FRotator GetMuzzleRotation() const override;
	virtual float GetCooldownTime() const override { return CooldownTime; }
	virtual FGameplayTagContainer GetCooldownTags() const override { return CooldownTags; }
	virtual void Destroy() override;
	virtual bool CanBeFired() const override;
	virtual void Fire(const FVector& muzzleLocation, const FRotator& muzzleRotation) const override;
	virtual void FireClient(bool isFirstPerson) override;

	virtual USoundBase* GetFireSound() const override { return FireSound; }
	virtual UParticleSystem* GetMuzzleEffect() const override { return MuzzleEffect; }
	
	virtual void SetCharacterOwner(ACharacterBase* character) override;

	virtual UTexture2D* GetCrossHairTexture() const override { return CrossHair; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
	void OnFire(bool isFirstPerson);

private:
	void SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const;
	FVector GetProjectileSpawnLocation(const FVector& location, const FRotator& rotation, float distanceFromMuzzleLocation) const;
	
	//FActiveGameplayEffectHandle mPickupIndicatorEffectHandle;
	
	void DebugDrawAiming() const;
};