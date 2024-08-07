// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "BulletShell.h"
#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupObject.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	mAmmo = MagazineCapacity;
}

bool AWeaponBase::CanBeFired() const
{
	return !IsMagazineEmpty();
}

void AWeaponBase::Fire(const FVector& targetLocation)
{
	//Only local (to the weapon firing)
	//Removed local only, we are setting a multicast so the fire has to be done on all characters (server-clients)
	//This way sound can be listened for every player.
	//BP_OnFire();
	
	//DrawDebugSphere(GetWorld(), targetLocation, 10, 10, FColor::Green, true);
	if(CanBeFired())
	{
		ServerFire(GetMuzzleLocation(), targetLocation);
		if(!HasAuthority())
		{
			//Consume ammo also in clients so they can see how ammo goes down event with lag.
			ConsumeAmmo();
			mPendingAmmoRPCACKs++;
		}
	}
}

EAmmoType AWeaponBase::GetAmmoType() const
{
	return ProjectileClass.GetDefaultObject()->GetAmmoType();	
}

void AWeaponBase::Reload(int ammoAmount)
{
	mAmmo += ammoAmount;
	check(mAmmo <= GetMagazineCapacity());
	if(const auto character = Cast<ACharacterBase>(GetOwner()))
	{
		RefreshAmmoHUD(character);
	}
	MulticastReload(mAmmo);
}

void AWeaponBase::ConsumeAmmo()
{
	mAmmo = FMath::Clamp(mAmmo-1, 0, MagazineCapacity);
}

void AWeaponBase::SetupLeftHandSocketTransform(const FVector& newLocation, const FRotator& newRotation)
{
	mLeftHandSocketTransform.SetLocation(newLocation);
	mLeftHandSocketTransform.SetRotation(FQuat(newRotation));
}

FVector AWeaponBase::GetMuzzleLocation() const
{
	return GetSocketMeshTransformBySocketName(MuzzleSocketName).GetLocation();
}

FRotator AWeaponBase::GetMuzzleRotation() const
{
	return GetSocketMeshTransformBySocketName(MuzzleSocketName).GetRotation().Rotator();
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

void AWeaponBase::ServerFire_Implementation(const FVector& muzzleLocation, const FVector& targetLocation)
{
	if(!HasAuthority())
	{
		return;
	}

	//DrawDebugSphere(GetWorld(), targetLocation, 5, 10, FColor::Blue, true);
	//DrawDebugLine(GetWorld(), muzzleLocation, targetLocation, FColor::Red, true);
	if(!SpawnProjectileServer(muzzleLocation, targetLocation - muzzleLocation))
	{
		return;
	}
	MulticastFire();
	ConsumeAmmo();
	UpdateAmmo(mAmmo);
	if(const auto character = Cast<ACharacterBase>(GetOwner()))
	{
		if(MuzzleGameplayEffectClass)
		{
			character->GetAbilitySystemComponentBase()->ApplyGameplayEffectToSelf(MuzzleGameplayEffectClass);
		}
		RefreshAmmoHUD(character);
	}
}

//This method will be called from Blueprint (Gameplay cue shoot that makes the visual effects of the weapon when firing)
void AWeaponBase::OnFire()
{
	SpawnBulletShell();
}

void AWeaponBase::MulticastReload_Implementation(int32 serverAmmo)
{
	if(const auto character = Cast<ACharacterBase>(GetOwner()))
	{
		if(character->IsLocallyControlled())
		{
			mAmmo = serverAmmo;
			RefreshAmmoHUD(character);
		}
	}
	BP_OnReload();
}

void AWeaponBase::StartAiming(const FVector& location, const FRotator& rotation)
{
	SetActorRotation(rotation);
}

void AWeaponBase::StopAiming()
{
}

FVector AWeaponBase::GetForwardVector() const
{
	return GetSocketMeshTransformBySocketName(MuzzleSocketName).GetRotation().GetForwardVector();
	//return GetActorTransform().GetRotation().GetForwardVector(); 
}

void AWeaponBase::UpdateAmmo_Implementation(int32 serverAmmo)
{
	if(HasAuthority())
		return;

	//LAG COMPENSATION
	//We set the ammo the server has
	mAmmo = serverAmmo;
	//decrement rpc sequence because we just recieve a rpc response
	mPendingAmmoRPCACKs--;
	//finally remove the pendingAmmoRPCACKs because are other shots client did but still pending to process by server
	mAmmo -= mPendingAmmoRPCACKs;
	
	if(const auto character = Cast<ACharacterBase>(GetOwner()))
	{
		RefreshAmmoHUD(character);
	}
}

void AWeaponBase::MulticastFire_Implementation()
{
	BP_OnFire();
}

bool AWeaponBase::SpawnProjectileServer(const FVector& muzzleLocation, const FVector& shootingDirection) const
{
	if (ProjectileClass == nullptr)
	{
		return false;
	}
	const auto location = GetProjectileSpawnLocation(muzzleLocation, shootingDirection, DistanceFromMuzzleLocation);
	FTransform transform;
	transform.SetLocation(location);
	transform.SetRotation(shootingDirection.Rotation().Quaternion());
	const auto projectile = utils::UtilsLibrary::LaunchProjectile(GetWorld(), ProjectileClass, transform, GetOwner(), Cast<APawn>(GetOwner()));
	return projectile != nullptr;
}

FTransform AWeaponBase::SaveLeftHandSocketTransform()
{
	check(!LeftHandSocketName.IsNone());
	mLeftHandSocketTransform = GetMesh()->GetSocketTransform(LeftHandSocketName, RTS_World);
	
	return mLeftHandSocketTransform;
}

FTransform AWeaponBase::GetMuzzleSocketTransform()
{
	check(!MuzzleSocketName.IsNone());
	return GetMesh()->GetSocketTransform(MuzzleSocketName, RTS_World);
}

FTransform AWeaponBase::GetCrosshairSocketTransform()
{
	check(!CrosshairSocketName.IsNone());
	return GetMesh()->GetSocketTransform(CrosshairSocketName, RTS_World);
}

void AWeaponBase::SpawnBulletShell() const
{
	if(!BulletShellClass)
	{
		return;
	}

	const auto socketTransform = GetSocketMeshTransformBySocketName(ShellSocketName);
	const auto bulletShell = GetWorld()->SpawnActor<ABulletShell>(
		BulletShellClass,
		socketTransform.GetLocation(),
		socketTransform.GetRotation().Rotator());
	bulletShell->ApplyEjectionImpulse(ShellEjectionImpulse);
}

void AWeaponBase::RefreshAmmoHUD(const ACharacterBase* character) const
{
	if(character->IsLocallyControlled())
	{
		Cast<UBattleRoyaleGameInstance>(GetGameInstance())->GetEventDispatcher()->OnRefreshAmmo.Broadcast(GetAmmo(), GetMagazineCapacity());
	}
}

FTransform AWeaponBase::GetSocketMeshTransformBySocketName(const FName& socketName) const
{
	if(const auto weaponMuzzleSocket = GetMesh()->GetSocketByName(socketName))
	{
		return weaponMuzzleSocket->GetSocketTransform(GetMesh());
	}

	UE_LOG(LogWeapon, Error, TEXT("[AWeaponBase::GetSocketMeshTransformBySocketName] socket with name %s not found"), *socketName.ToString());

	return FTransform::Identity;
}
