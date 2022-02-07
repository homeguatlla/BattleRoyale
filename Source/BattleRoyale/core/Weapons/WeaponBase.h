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
	
	UPROPERTY(EditDefaultsOnly, Category = Muzzle)
	FName MuzzleSocketName;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AProjectileBase> ProjectileClass;

public:	
	AWeaponBase();

	//virtual USkeletalMeshComponent* GetMesh() const override { return Mesh; }

	virtual FVector GetMuzzleLocation() const override;
	virtual FRotator GetMuzzleRotation() const override;
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules,
			const FName& socketName) override;
	virtual bool CanBeFired() const override;
	virtual void Fire() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
private:
	void SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const;
};
