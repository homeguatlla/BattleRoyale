// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
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

bool UCombatComponent::EquipWeapon(TScriptInterface<IPickupObject> pickableObject, const FName& socketName)
{
	if(pickableObject.GetObject() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::EquipWeapon] weapon is null"), *GetName());
		return false;
	}
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);
	
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	const auto isAttached = pickableObject->AttachToComponent(
		character->GetMesh(),
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		socketName);
	
	if(!isAttached)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::EquipWeapon] weapon not attached to the character"), *GetName());
		return false;
	}
	
	mEquipedWeapon = TScriptInterface<IWeapon>(pickableObject.GetObject());
	mEquipedWeapon->SetWeaponState(EWeaponState::Equipped);	
	mEquipedWeapon->SetCharacterOwner(character);
	
	return true;
}

bool UCombatComponent::UnEquipWeapon() const
{
	TScriptInterface<IPickupObject>(mEquipedWeapon.GetObject())->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	mEquipedWeapon->Destroy();

	return true;
}

TScriptInterface<IWeapon> UCombatComponent::GetEquippedWeapon() const
{
	return mEquipedWeapon;
}