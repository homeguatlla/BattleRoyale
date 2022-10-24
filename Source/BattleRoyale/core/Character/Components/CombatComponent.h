// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class IPickupObject;
class IWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	TScriptInterface<IWeapon> mEquipedWeapon;
	
public:	
	// Sets default values for this component's properties
	UCombatComponent();

	bool EquipWeapon(TScriptInterface<IPickupObject> pickableObject, const FName& socketName);
	bool UnEquipWeapon() const;
	TScriptInterface<IWeapon> GetEquippedWeapon() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
};
