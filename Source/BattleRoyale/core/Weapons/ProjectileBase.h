// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "IProjectile.h"
#include "GameFramework/Actor.h"
#include "ProjectileBase.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class IICharacter;

UCLASS(config=Game)
class AProjectileBase : public AActor, public IProjectile
{
	GENERATED_BODY()

	/** Sphere collision component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category= "Projectile", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
	
	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* ExplosionEffect;

	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	UParticleSystem* TrailEffect;
	
	// Particle used when the projectile impacts against another object and explodes.
	UPROPERTY(EditDefaultsOnly, Category = "Projectile")
	USoundBase* ExplosionSound;
	
	UPROPERTY(EditAnywhere, Category = "Projectile")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	float ProjectileEjectionImpulse = 100.0f;

	
	
public:
	AProjectileBase();

	virtual void Destroyed() override;
	
	virtual UParticleSystem* GetExplosionEffect() const override { return ExplosionEffect; }
	virtual USoundBase* GetExplosionSound() const override { return ExplosionSound; }
	virtual UParticleSystem* GetTrailEffect() const override { return TrailEffect; }
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Projectile")
	void OnExplode();

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

protected:
	void ApplyDamageToCharacter(const IICharacter* character) const;
	virtual void DoApplyDamageFrom(AActor* hitActor, const FVector& center) const;

	UPROPERTY(EditAnywhere, Category = "Projectile")
	bool IsDebugEnabled { false };
};

