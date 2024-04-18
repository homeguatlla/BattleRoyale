// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectileBase.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Utils/PhysicalMaterial/PhysicalMaterialBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "GameFramework/PlayerState.h"
#include "Components/SphereComponent.h"
#include "GameFramework/DamageType.h"

AProjectileBase::AProjectileBase() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	
	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
	bReplicates = true;
}

void AProjectileBase::BeginPlay()
{
	Super::BeginPlay();
	//this doesn't work on constructor because in constructor the Projectile has the authority. Later it is changed if is not the server.
	if(HasAuthority())
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);		// set up a notification for when this component hits something blocking
	}
}

void AProjectileBase::Destroyed()
{
	//This method will be executed on all players, although the hit is only implemented on server
	//The replicated proyectile will be destroyed too.
	Super::Destroyed();
	OnExplode(mImpactPhysicalMaterial);
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ensureMsgf(HasAuthority(), TEXT("Only Server can Receive OnHit for projectiles"));
	
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		// Only add impulse and destroy projectile if we hit a physics
		if(OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * ProjectileEjectionImpulse, GetActorLocation());
		}

		if(OtherComp->GetNumMaterials() > 0)
		{
			mImpactPhysicalMaterial = Cast<UPhysicalMaterialBase>(OtherComp->GetMaterial(0)->GetPhysicalMaterial());
		}
		
		DoApplyDamageFrom(OtherActor, Hit.ImpactPoint);
		
		/*
		 * Unreal way to apply damage
		if(OtherActor->CanBeDamaged())
		{
			AController* controller = nullptr;
			if(GetInstigator())
			{
				controller = GetInstigator()->Controller;
			}
			//This method is executed on server only
			UGameplayStatics::ApplyPointDamage(
				OtherActor,
				Damage,
				NormalImpulse,
				Hit,
				controller, //the controller of the character who shoot
				this, //projectile is causing damage
				DamageType);
		}*/
		Destroy();
	}
}

void AProjectileBase::ApplyDamageToCharacter(const IICharacter* character) const
{
	if(const auto instigatorPlayerState = Cast<IIPlayerState>(GetInstigator()->GetPlayerState()))
	{
		//On instant effects, the handle returned is always not valid. So don't check if the effect has been properly applied.
		instigatorPlayerState->GetAbilitySystemComponentInterface()->ApplyGameplayEffectToTarget(DamageEffect, character);
	}
}

void AProjectileBase::DoApplyDamageFrom(AActor* hitActor, const FVector& center) const
{
	if(const auto hurtCharacter = Cast<IICharacter>(hitActor))
	{
		ApplyDamageToCharacter(hurtCharacter);
	}
}
