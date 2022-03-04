// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IWeapon.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS(Blueprintable)
class BATTLEROYALE_API AWeaponBase : public AActor, public IIWeapon
{
	GENERATED_BODY()

	/** Weapon mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh;
	
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
	float DistanceFromMuzzleLocation = { 20.0f};

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	bool IsDebugEnabled { false };
	
public:	
	AWeaponBase();
	
	virtual void Tick( float DeltaSeconds ) override;
	
	//virtual USkeletalMeshComponent* GetMesh() const override { return Mesh; }

	virtual FVector GetMuzzleLocation() const override;
	virtual FRotator GetMuzzleRotation() const override;
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules,
	                               const FName& socketName) override;
	virtual bool CanBeFired() const override;
	virtual void Fire() const override;
	virtual void FireClient(bool isFirstPerson) override;

	virtual USoundBase* GetFireSound() const override { return FireSound; }
	virtual UParticleSystem* GetMuzzleEffect() const override { return MuzzleEffect; }
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Weapon")
	void OnFire(bool isFirstPerson);

private:
	void SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const;
	FVector GetProjectileSpawnLocation(float distanceFromMuzzleLocation) const;
	
	void DebugDrawAiming() const;
};
