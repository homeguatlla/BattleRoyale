// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "BattleRoyale/BattleRoyale.h"
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

			auto direction = GetMuzzleRotation().RotateVector(FVector::ForwardVector);
			direction.Normalize();
			const auto muzzleLocationWithOffset = GetMuzzleLocation() +  direction * DistanceFromMuzzleLocation;

			// spawn the projectile at the muzzle
			World->SpawnActor<AProjectileBase>(ProjectileClass, muzzleLocationWithOffset, muzzleRotation, ActorSpawnParams);
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

void AWeaponBase::Fire() const
{
	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void AWeaponBase::ServerFire() const
{
	SpawnProjectile(GetMuzzleLocation(), GetMuzzleRotation());
}

void AWeaponBase::DebugDrawAiming() const
{
	auto direction = GetMuzzleRotation().RotateVector(FVector::ForwardVector);
	direction.Normalize();
	const auto muzzleLocation = GetMuzzleLocation() +  direction * DistanceFromMuzzleLocation;
	
	DrawDebugSphere(GetWorld(), GetMuzzleLocation(), 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	const auto muzzleLocationFar = GetMuzzleLocation() +  direction * 500;
	DrawDebugLine(GetWorld(), muzzleLocation, muzzleLocationFar, FColor::Blue, false);
}
