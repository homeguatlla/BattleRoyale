// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "BlueprintGameplayTagLibrary.h"
#include "DrawDebugHelpers.h"
#include "ProjectileBase.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupIndicator.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/SkeletalMeshSocket.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;
	
	// Create a gun mesh component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	Mesh->bCastDynamicShadow = false;
	Mesh->CastShadow = false;

	SetRootComponent(Mesh);
	
	Mesh->SetCollisionResponseToChannels(ECollisionResponse::ECR_Block);
	//Ignoring the collision with pawn, so characters can walk over.
	Mesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	//Not collision from construction
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//Area sphere is used to detect overlaps with characters to show something and be able to pick up
	//Disabled for now. We only want to be enabled on server
	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(RootComponent);
	AreaSphere->SetCollisionResponseToChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if(HasAuthority())
	{
		//Enable the Area Sphere 
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);

		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereOverlapServer);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::AWeaponBase::OnSphereEndOverlapServer);
	}
}

void AWeaponBase::OnSphereOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if(const auto character = Cast<ACharacterBase>(OtherActor))
	{
		const auto playerState = Cast<APlayerStateBase>(character->GetPlayerState());
		if(!playerState)
		{
			return;
		}
		
		utils::UtilsLibrary::SendGameplayEventWithTargetData<FTargetDataPickupIndicator>(
			character,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_PICKUP_INDICATOR),
			new FTargetDataPickupIndicator(GetPickupWidgetLocation()));

		//Si enviamos un efecto también funciona, incluso podemos dejar la habilidad como Local Only,
		//pero no podemos pasar parámetros
		//mPickupIndicatorEffectHandle = playerState->GetAbilitySystemComponentInterface()->ApplyGameplayEffectToSelf(PickupIndicatorEffect);
	}
}

void AWeaponBase::OnSphereEndOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if(const auto character = Cast<ACharacterBase>(OtherActor))
	{
		const auto playerState = Cast<APlayerStateBase>(character->GetPlayerState());
		if(!playerState)
		{
			return;
		}
		//Remove the ability with tag pickup indicator
		FGameplayTagContainer cancelTags;
		UBlueprintGameplayTagLibrary::AddGameplayTag(cancelTags, FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP_INDICATOR));
		playerState->GetAbilitySystemComponent()->CancelAbilities(&cancelTags);

		//En caso que enviemos effecto hay que quitarlo
		//playerState->GetAbilitySystemComponentInterface()->RemoveGameplayEffect(mPickupIndicatorEffectHandle);
	}
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
	if(const auto weaponMuzzleSocket = Mesh->GetSocketByName(MuzzleSocketName))
	{
		return weaponMuzzleSocket->GetSocketLocation(Mesh);
	}

	UE_LOG(LogWeapon, Error, TEXT("[%s][AWeaponBase::GetMuzzleLocation] muzzle socket not found"), *MuzzleSocketName.ToString());

	return FVector::ZeroVector;
}

FRotator AWeaponBase::GetMuzzleRotation() const
{
	if(const auto weaponMuzzleSocket = Mesh->GetSocketByName(MuzzleSocketName))
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

bool AWeaponBase::AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName)
{
	if(Mesh == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::AttachToComponent] weapon has no mesh"), *GetName());
		return false;
	}
	
	return Mesh->AttachToComponent(meshComponent, attachmentRules, socketName);
}

void AWeaponBase::DetachFromComponent(const FDetachmentTransformRules& rules)
{
	if(Mesh == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::DeattachToComponent] weapon has no mesh"), *GetName());
		return;
	}
	
	Mesh->DetachFromComponent(rules);
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

FVector AWeaponBase::GetPickupWidgetLocation() const
{
	check(Mesh);
	const auto bounds = Mesh->GetLocalBounds();
	const auto height = bounds.BoxExtent.Z * 2.0f;
	const auto origin = bounds.Origin;
	return GetActorLocation() + origin + FVector(0.0f, 0.0f, height);
}

void AWeaponBase::SetCharacterOwner(ACharacterBase* character)
{
	SetOwner(character);
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

void AWeaponBase::DebugDrawAiming() const
{
	const auto muzzleLocation = GetProjectileSpawnLocation(GetMuzzleLocation(), GetMuzzleRotation(), DistanceFromMuzzleLocation);
	
	DrawDebugSphere(GetWorld(), GetMuzzleLocation(), 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	DrawDebugLine(GetWorld(), muzzleLocation, GetProjectileSpawnLocation(GetMuzzleLocation(), GetMuzzleRotation(), 500), FColor::Blue, false);
}
