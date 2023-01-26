// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, mEquippedWeapon);
	DOREPLIFETIME(UCombatComponent, mIsAiming);
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(IsDebugEnabled)
	{
		DebugDrawAiming();
	}
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);

	mAimWalkSpeed = character->GetMinWalkSpeed();
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
	
	mEquippedWeapon = weapon;
	mEquippedWeapon->SetCharacterOwner(character);
	SetupLeftHandSocketTransform(character);
	
	return true;
}

bool UCombatComponent::UnEquipWeapon() const
{
	mEquippedWeapon->Destroy(); //TODO esto está mal creo debería dejarla tal cual y hacerle un dropped en el character

	return true;
}

TScriptInterface<IWeapon> UCombatComponent::GetEquippedWeapon() const
{
	return mEquippedWeapon;
}

bool UCombatComponent::CanShoot() const
{
	return mEquippedWeapon && mEquippedWeapon->CanBeFired();
}

void UCombatComponent::StartAiming()
{
	mIsAiming = true;
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);

	character->GetCharacterMovement()->MaxWalkSpeed = mAimWalkSpeed;
	
	character->GetAbilitySystemComponentBase()->AddGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
	character->GetAbilitySystemComponentBase()->CancelAbilitiesWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT)));
}

void UCombatComponent::StopAiming()
{
	mIsAiming = false;
	const auto character = Cast<ACharacterBase>(GetOwner());
	check(character);
	character->GetCharacterMovement()->MaxWalkSpeed = character->GetMaxWalkSpeed();
	character->GetAbilitySystemComponentBase()->RemoveGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
}

bool UCombatComponent::CanAim() const
{
	return HasWeaponEquipped() && !mIsAiming;
}

void UCombatComponent::Shoot() const
{
	if(!HasWeaponEquipped())
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::Shoot] There is no weapon equipped"), *GetName());
		return;
	}

	const auto shootingTargetLocation = CalculateShootingTarget();
	const auto weapon = GetEquippedWeapon();
	weapon->Fire(shootingTargetLocation);
}

void UCombatComponent::SetupLeftHandSocketTransform(const ACharacterBase* character) const
{
	if(!HasWeaponEquipped())
		return;

	const auto leftHandTransform = GetEquippedWeapon()->SaveLeftHandSocketTransform();
	FVector newPosition;
	FRotator newRotator;
	character->GetMesh()->TransformToBoneSpace(
		FName("hand_r"),
		leftHandTransform.GetLocation(),
		FRotator::ZeroRotator,
		newPosition,
		newRotator);

	GetEquippedWeapon()->SetupLeftHandSocketTransform(newPosition, newRotator);
}


FVector UCombatComponent::CalculateShootingTarget() const
{
	const auto playerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	const auto hitResult = utils::UtilsLibrary::TraceLineSingleByChannelToCrossHairs(GetWorld(), playerController, MaxShootingDistance);

	if(!hitResult.IsValidBlockingHit())
	{
		return hitResult.TraceEnd;
	}
	return hitResult.ImpactPoint;
}

void UCombatComponent::DebugDrawAiming() const
{
	if(!HasWeaponEquipped())
	{
		return;
	}
	const auto muzzleLocation = GetEquippedWeapon()->GetMuzzleLocation();
	const auto shootingTargetLocation = CalculateShootingTarget();
	DrawDebugSphere(GetWorld(), muzzleLocation, 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	DrawDebugLine(GetWorld(), muzzleLocation, shootingTargetLocation, FColor::Blue, false);
}