// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AmmoTypes.h"
#include "BattleRoyale/core/PickableObjects/PickableObjectBase.h"
#include "Ammo.generated.h"

UCLASS()
class BATTLEROYALE_API AAmmo : public APickableObjectBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	EAmmoType AmmoType;
	
public:
	
	// Sets default values for this actor's properties
	AAmmo();

	EAmmoType GetAmmoType() const { return AmmoType; }
};
