// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IWeapon.h"
#include "GameFramework/Actor.h"
#include "WeaponBase.generated.h"

UCLASS()
class BATTLEROYALE_API AWeaponBase : public AActor, public IIWeapon
{
	GENERATED_BODY()

	/** Weapon mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* Mesh;
	
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	FName MuzzleSocketName;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class AProjectileBase> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category= Projectile)
	USoundBase* FireSound;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	UParticleSystem* MuzzleEffect;
	
	UPROPERTY(EditDefaultsOnly, Category= Projectile)
	float DistanceFromMuzzleLocation = { 20.0f};

	UPROPERTY(EditDefaultsOnly, Category= Projectile)
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
	
private:
	void SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const;
	FVector GetProjectileSpawnLocation(float distanceFromMuzzleLocation) const;
	
	void DebugDrawAiming() const;
};
