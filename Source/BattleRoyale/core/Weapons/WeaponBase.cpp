// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupObject.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
}

void AWeaponBase::Tick( float DeltaSeconds )
{
	Super::Tick(DeltaSeconds);

	if(IsDebugEnabled)
	{
		DebugDrawAiming();
	}
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

void AWeaponBase::SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

			const auto location = GetProjectileSpawnLocation(muzzleLocation, muzzleRotation, DistanceFromMuzzleLocation);

			/*const FString Message = FString::Printf(TEXT("Location: %s, Rotation: %s"),*location.ToString(), *muzzleRotation.ToString());
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, Message);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Black, Mesh->GetName());
				
			DrawDebugLine(GetWorld(), location, GetProjectileSpawnLocation(muzzleLocation, muzzleRotation, 500), FColor::Red, false, 60);
			DrawDebugSphere(GetWorld(), muzzleLocation, 5, 10, FColor::Blue, true, -1, 0, 3);*/
			// spawn the projectile at the muzzle
			if(const auto projectile = World->SpawnActor<AProjectileBase>(ProjectileClass, location, muzzleRotation, ActorSpawnParams))
			{
				const auto owner = GetOwner();
				UE_LOG(LogTemp, Warning, TEXT("WeaponBase::SpawnProjectile %d"), owner->GetLocalRole());
				projectile->SetInstigator(Cast<APawn>(GetOwner()));
			}
			else
			{
				UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::SpawnProjectile] Couldn't spawn the projectile"), *GetName());
			}
		}
	}
}

FTransform AWeaponBase::GetLeftHandSocketTransform()
{
	return mLeftHandSocketTransform;
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


void AWeaponBase::FireClient(bool isFirstPerson)
{
	OnFire(isFirstPerson);
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

void AWeaponBase::OnFire_Implementation(bool isFirstPerson)
{
}

void AWeaponBase::Fire(const FVector& muzzleLocation, const FRotator& muzzleRotation) const
{
	// try and fire a projectile:
	//the server has the weapon in FP1, but for the clients it has the weapons as 3P
	//so, we need when shooting send to the server our weapon location and rotation
	//because server will get wrong location and rotation for clients
	SpawnProjectile(muzzleLocation, muzzleRotation);
}

FVector AWeaponBase::GetProjectileSpawnLocation(const FVector& location, const FRotator& rotation, float distanceFromMuzzleLocation) const
{
	auto direction = rotation.RotateVector(FVector::ForwardVector);
	direction.Normalize();
	return location +  direction * distanceFromMuzzleLocation;
}

FTransform AWeaponBase::SaveLeftHandSocketTransform()
{
	check(!LeftHandSocketName.IsNone());
	mLeftHandSocketTransform = GetMesh()->GetSocketTransform(LeftHandSocketName, RTS_World);
	
	return mLeftHandSocketTransform;
}

void AWeaponBase::DebugDrawAiming() const
{
	const auto muzzleLocation = GetProjectileSpawnLocation(GetMuzzleLocation(), GetMuzzleRotation(), DistanceFromMuzzleLocation);
	
	DrawDebugSphere(GetWorld(), GetMuzzleLocation(), 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	DrawDebugLine(GetWorld(), muzzleLocation, GetProjectileSpawnLocation(GetMuzzleLocation(), GetMuzzleRotation(), 500), FColor::Blue, false);
}
