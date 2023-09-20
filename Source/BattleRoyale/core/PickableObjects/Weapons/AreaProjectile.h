// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileBase.h"
#include "AreaProjectile.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API AAreaProjectile : public AProjectileBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	float ExplosionRadius = 150.0f; //3 meters

protected:
	virtual void DoApplyDamageFrom(AActor* hitActor, const FVector& center) const override;
};
