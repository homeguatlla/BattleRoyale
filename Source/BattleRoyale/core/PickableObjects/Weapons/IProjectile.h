// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IProjectile.generated.h"

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UProjectile : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IProjectile
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Projectile")
	virtual UParticleSystem* GetExplosionEffect() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Projectile")
	virtual USoundBase* GetExplosionSound() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Projectile")
	virtual UParticleSystem* GetTrailEffect() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "Projectile")
	virtual EAmmoType GetAmmoType() const = 0;
};