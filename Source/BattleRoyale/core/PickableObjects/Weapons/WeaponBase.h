// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IWeapon.h"
#include "BattleRoyale/core/PickableObjects/PickableObjectBase.h"
#include "BattleRoyale/core/PickableObjects/Ammo/AmmoTypes.h"
#include "WeaponBase.generated.h"

class ICrosshair;
class UGameplayEffect;
class IICharacter;

UCLASS(Blueprintable)
class BATTLEROYALE_API AWeaponBase : public APickableObjectBase, public IWeapon
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName MuzzleSocketName;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName ShellSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float ShellEjectionImpulse = 100.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName CrosshairSocketName;
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class AProjectileBase> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<class ABulletShell> BulletShellClass;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<UGameplayEffect> MuzzleGameplayEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TScriptInterface<ICrosshair> Crosshair;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float DistanceFromMuzzleLocation = { 20.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float CooldownTime = {3.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FGameplayTagContainer CooldownTags;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	FName LeftHandSocketName;

	UPROPERTY(EditDefaultsOnly, Category="Weapon")
	UAnimationAsset* ShootingAnimation;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool AutomaticFire = false;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	float AutomaticFireDelay = 0.0f;
	
	//This is where the left hand goes on the weapon.
	FTransform mLeftHandSocketTransform;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float ZoomedFOV = 30.0f;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	float ZoomInterpolationSpeed = 20.0f;

	UPROPERTY(EditDefaultsOnly, Replicated, Category = "Weapon")
	int32 mAmmo;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	int32 MagazineCapacity;
	
public:	
	AWeaponBase();

	virtual FVector GetMuzzleLocation() const override;
	virtual FRotator GetMuzzleRotation() const override;
	virtual float GetCooldownTime() const override { return CooldownTime; }
	virtual FGameplayTagContainer GetCooldownTags() const override { return CooldownTags; }
	virtual FTransform GetLeftHandSocketTransform() override;
	virtual FTransform SaveLeftHandSocketTransform() override;
	virtual FTransform GetMuzzleSocketTransform() override;
	virtual FTransform GetCrosshairSocketTransform() override;
	
	virtual void Destroy() override;
	virtual bool CanBeFired() const override;
	virtual void Fire(const FVector& targetLocation) override;
	virtual void OnFire() override;//This method will be called from Blueprint (Gameplay cue shoot that makes the visual effects of the weapon when firing)
	virtual bool IsAutomaticFireEnabled() const override { return AutomaticFire; }
	virtual float GetAutomaticFireDelay() const override { return AutomaticFireDelay; }
	virtual int32 GetAmmo() const override { return mAmmo; }
	virtual int32 GetMagazineCapacity() const override { return MagazineCapacity; }
	virtual EAmmoType GetAmmoType() const override;
	virtual bool IsMagazineFull() const override { return MagazineCapacity == mAmmo; }
	virtual bool HasAmmo() const override{ return mAmmo > 0; }
	virtual void Reload(int ammoAmount) override;
	
	virtual UAnimationAsset* GetShootingAnimation() const override { return ShootingAnimation; }
	
	virtual void SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation) override;
	virtual TScriptInterface<ICrosshair> GetCrosshairWidget() const override { return Crosshair; }
	
	virtual float GetZoomedFOV() const override { return ZoomedFOV; }
	virtual float GetZoomInterpolationSpeed() const override { return ZoomInterpolationSpeed; }

	virtual void StartAiming(const FVector& location, const FRotator& rotation) override;
	virtual void StopAiming() override;
	virtual FVector GetForwardVector() const override;
	
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "Weapon", meta = (DisplayName = OnFire))
	void BP_OnFire();

	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector& muzzleLocation, const FVector& targetLocation);
	
	bool SpawnProjectileServer(const FVector& muzzleLocation, const FVector& shootingDirection) const;
	FVector GetProjectileSpawnLocation(const FVector& location, const FVector& direction, float distanceFromMuzzleLocation) const;

	FTransform GetSocketMeshTransformBySocketName(const FName& socketName) const;
	void SpawnBulletShell() const;

	bool IsMagazineEmpty() const { return mAmmo <= 0; }
};
