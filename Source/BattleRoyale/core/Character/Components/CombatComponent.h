// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

	UPROPERTY(Replicated)
	TScriptInterface<IWeapon> mEquipedWeapon;

	UPROPERTY(Replicated)
	bool mIsAiming;
	
public:	
	// Sets default values for this component's properties
	UCombatComponent();

	bool EquipWeapon(TScriptInterface<IWeapon> weapon, const FName& socketName);
	bool UnEquipWeapon() const;
	TScriptInterface<IWeapon> GetEquippedWeapon() const;
	bool HasWeaponEquipped() const { return GetEquippedWeapon() != nullptr; }
	
	bool IsAiming() const { return mIsAiming; }
	void StartAiming();
	void StopAiming();
	bool CanAim() const;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
};
