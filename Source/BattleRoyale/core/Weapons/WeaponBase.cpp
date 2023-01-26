// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupObject.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponBase::Destroy()
{
	AActor::Destroy();
}

bool AWeaponBase::CanBeFired() const
{
	//TODO has bullets?
	return true;
}

void AWeaponBase::Fire(const FVector& targetLocation)
{
	//BP_OnFire();
	//DrawDebugSphere(GetWorld(), targetLocation, 10, 10, FColor::Green, true);
	ServerFire(GetMuzzleLocation(), targetLocation);
}

void AWeaponBase::SetCharacterOwner(ACharacterBase* character)
{
	SetOwner(character);
}

void AWeaponBase::SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation)
{
	mLeftHandSocketTransform.SetLocation(newLocation);
	mLeftHandSocketTransform.SetRotation(FQuat(newRotation));
}

FVector AWeaponBase::GetMuzzleLocation() const
{
	if(const auto weaponMuzzleSocket = GetMesh()->GetSocketByName(MuzzleSocketName))
	{
		return weaponMuzzleSocket->GetSocketLocation(GetMesh());
	}

	UE_LOG(LogWeapon, Error, TEXT("[%s][AWeaponBase::GetMuzzleLocation] muzzle socket not found"), *MuzzleSocketName.ToString());

	return FVector::ZeroVector;
}

FRotator AWeaponBase::GetMuzzleRotation() const
{
	if(const auto weaponMuzzleSocket = GetMesh()->GetSocketByName(MuzzleSocketName))
	{
		return weaponMuzzleSocket->GetSocketTransform(GetMesh()).GetRotation().Rotator();
	}
	else
	{
		UE_LOG(LogWeapon, Error, TEXT("[%s][AWeaponBase::GetMuzzleRotation] muzzle socket not found"), *MuzzleSocketName.ToString());
	}
	return FRotator::ZeroRotator;
}

FTransform AWeaponBase::GetLeftHandSocketTransform()
{
	return mLeftHandSocketTransform;
}

FVector AWeaponBase::GetProjectileSpawnLocation(const FVector& location, const FVector& direction, float distanceFromMuzzleLocation) const
{
	auto normalizedDirection= direction;
	normalizedDirection.Normalize();
	return location + normalizedDirection * distanceFromMuzzleLocation;
}

void AWeaponBase::ServerFire_Implementation(const FVector& muzzleLocation, const FVector& targetLocation) const
{
	if(!HasAuthority())
	{
		return;
	}

	//DrawDebugSphere(GetWorld(), targetLocation, 5, 10, FColor::Blue, true);
	//DrawDebugLine(GetWorld(), muzzleLocation, targetLocation, FColor::Red, true);
	SpawnProjectileServer(muzzleLocation, targetLocation - muzzleLocation);

	if(const auto character = Cast<IICharacter>(GetOwner()))
	{
		if(MuzzleGameplayEffect)
		{
			character->GetAbilitySystemComponentBase()->ApplyGameplayEffectToSelf(MuzzleGameplayEffect);
		}
	}
}

void AWeaponBase::SpawnProjectileServer(const FVector& muzzleLocation, const FVector& shootingDirection) const
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			const auto location = GetProjectileSpawnLocation(muzzleLocation, shootingDirection, DistanceFromMuzzleLocation);
			
			//DrawDebugSphere(GetWorld(), location, 5, 10, FColor::Blue, true);
			
			// spawn the projectile at the muzzle
			if(const auto projectile = World->SpawnActor<AProjectileBase>(ProjectileClass, location, shootingDirection.Rotation(), ActorSpawnParams))
			{
				const auto owner = GetOwner();
				projectile->SetInstigator(Cast<APawn>(GetOwner()));
			}
			else
			{
				UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::SpawnProjectile] Couldn't spawn the projectile"), *GetName());
			}
		}
	}
}

FTransform AWeaponBase::SaveLeftHandSocketTransform()
{
	check(!LeftHandSocketName.IsNone());
	mLeftHandSocketTransform = GetMesh()->GetSocketTransform(LeftHandSocketName, RTS_World);
	
	return mLeftHandSocketTransform;
}
