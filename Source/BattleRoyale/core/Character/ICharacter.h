// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ICharacter.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UICharacter : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IICharacter
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual USkeletalMeshComponent* GetWeaponMesh() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsCharacterValid() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FVector GetCurrentVelocity() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FRotator GetCurrentRotation() const = 0;

	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual FRotator GetCurrentControlRotation() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "ICharacter")
	virtual bool IsFalling() const  = 0;
};