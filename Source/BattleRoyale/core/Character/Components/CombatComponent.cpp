// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, mEquipedWeapon);
	DOREPLIFETIME(UCombatComponent, mIsAiming);
}

bool UCombatComponent::EquipWeapon(TScriptInterface<IWeapon> weapon, const FName& socketName)
{
	if(weapon == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::EquipWeapon] weapon is null"), *GetName());
		return false;
	}
	
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);
	
	mEquipedWeapon = weapon;
	mEquipedWeapon->SetCharacterOwner(character);
	
	return true;
}

bool UCombatComponent::UnEquipWeapon() const
{
	mEquipedWeapon->Destroy(); //TODO esto está mal creo debería dejarla tal cual y hacerle un dropped en el character

	return true;
}

TScriptInterface<IWeapon> UCombatComponent::GetEquippedWeapon() const
{
	return mEquipedWeapon;
}

bool UCombatComponent::CanShoot() const
{
	return mEquipedWeapon && mEquipedWeapon->CanBeFired();
}

void UCombatComponent::StartAiming()
{
	mIsAiming = true;
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);
	character->GetAbilitySystemComponentBase()->AddGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
	character->GetAbilitySystemComponentBase()->CancelAbilitiesWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT)));
}

void UCombatComponent::StopAiming()
{
	mIsAiming = false;
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);
	character->GetAbilitySystemComponentBase()->RemoveGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
}

bool UCombatComponent::CanAim() const
{
	return HasWeaponEquipped() && !mIsAiming;
}
