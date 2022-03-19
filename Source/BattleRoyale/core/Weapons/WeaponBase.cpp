// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Create a gun mesh component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	Mesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	//Set to avoid a warning
	SetRootComponent(Mesh);
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
	const auto weaponMuzzleSocket = Mesh->GetSocketByName(MuzzleSocketName);
	if(weaponMuzzleSocket)
	{
		return weaponMuzzleSocket->GetSocketLocation(Mesh);
	}
	else
	{
		UE_LOG(LogWeapon, Error, TEXT("[%s][AWeaponBase::GetMuzzleLocation] muzzle socket not found"), *MuzzleSocketName.ToString());
	}
	return FVector::ZeroVector;
}

FRotator AWeaponBase::GetMuzzleRotation() const
{
	const auto weaponMuzzleSocket = Mesh->GetSocketByName(MuzzleSocketName);
	if(weaponMuzzleSocket)
	{
		return weaponMuzzleSocket->GetSocketTransform(Mesh).GetRotation().Rotator();
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
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			const auto projectile = World->SpawnActor<AProjectileBase>(ProjectileClass, GetProjectileSpawnLocation(DistanceFromMuzzleLocation), muzzleRotation, ActorSpawnParams);
			if(projectile)
			{
				projectile->SetInstigator(Cast<APawn>(GetOwner()));
			}
			else
			{
				UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::SpawnProjectile] Couldn't spawn the projectile"), *GetName());
			}
		}
	}
}

bool AWeaponBase::AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName)
{
	if(Mesh == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::EquipWeapon] weapon has no mesh"), *GetName());
		return false;
	}
	
	return Mesh->AttachToComponent(meshComponent, attachmentRules, socketName);
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

void AWeaponBase::OnFire_Implementation(bool isFirstPerson)
{
}

void AWeaponBase::Fire() const
{
	// try and fire a projectile:
	//the server has the weapon in FP1, but for the clients it has the weapons as 3P
	//so, we need when shooting send to the server our weapon location and rotation
	//because server will get wrong location and rotation for clients
	SpawnProjectile(GetMuzzleLocation(), GetMuzzleRotation());
}

FVector AWeaponBase::GetProjectileSpawnLocation(float distanceFromMuzzleLocation) const
{
	auto direction = GetMuzzleRotation().RotateVector(FVector::ForwardVector);
	direction.Normalize();
	return GetMuzzleLocation() +  direction * distanceFromMuzzleLocation;
}

void AWeaponBase::DebugDrawAiming() const
{
	const auto muzzleLocation = GetProjectileSpawnLocation(DistanceFromMuzzleLocation);
	
	DrawDebugSphere(GetWorld(), GetMuzzleLocation(), 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	DrawDebugLine(GetWorld(), muzzleLocation, GetProjectileSpawnLocation(500), FColor::Blue, false);
}
