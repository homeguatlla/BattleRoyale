// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "IProjectile.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class AProjectileBase : public AActor, public IProjectile
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* ExplosionEffect;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	USoundBase* ExplosionSound;
	
	//The damage type and damage that will be done by this projectile
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<class UDamageType> DamageType;

	//The damage dealt by this projectile.
	UPROPERTY(EditAnywhere, Category="Projectile")
	float Damage;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<UGameplayEffect> DamageEffect;
	
public:
	AProjectileBase();

	virtual void Destroyed() override;

	virtual UParticleSystem* GetExplosionEffect() const override { return ExplosionEffect; }
	virtual USoundBase* GetExplosionSound() const override { return ExplosionSound; }
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnExplode();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }
};

