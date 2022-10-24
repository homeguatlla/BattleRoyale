// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


class IPickableObject;
class IIWeapon;
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	TScriptInterface<IIWeapon> mEquipedWeapon;
	
public:	
	// Sets default values for this component's properties
	UCombatComponent();

	bool EquipWeapon(TScriptInterface<IPickableObject> pickableObject, const FName& socketName);
	bool UnEquipWeapon() const;
	TScriptInterface<IIWeapon> GetEquippedWeapon() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	
};
