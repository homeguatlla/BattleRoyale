// Copyright Epic Games, Inc. All Rights Reserved.

#include "ProjectileBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

AProjectileBase::AProjectileBase() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");

	if(HasAuthority())
	{
		CollisionComp->OnComponentHit.AddDynamic(this, &AProjectileBase::OnHit);		// set up a notification for when this component hits something blocking
	}
	
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

	//Setup damage
	DamageType = UDamageType::StaticClass();
	Damage = 10;
	
	bReplicates = true;
}

void AProjectileBase::Destroyed()
{
	Super::Destroyed();

	OnExplode();
}

void AProjectileBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr))
	{
		// Only add impulse and destroy projectile if we hit a physics
		if(OtherComp->IsSimulatingPhysics())
		{
			OtherComp->AddImpulseAtLocation(GetVelocity() * 100.0f, GetActorLocation());
		}

		/*
		 TODO: quizá podríamos comprobar si el proyectil puede hacer daño al other actor?? quizá no es el mejor lugar
		 *const auto gameMode = GetGameModeServer();
		if(!gameMode->CanPlayerCauseDamageTo(killer, victim))
		*/
		if(OtherActor->CanBeDamaged())
		{
			if(const auto hurtCharacter = Cast<IICharacter>(OtherActor))
			{
				const auto gameplayAbilityInterface = hurtCharacter->GetAbilitySystemComponentBase();
				const auto damageEffectHandle = gameplayAbilityInterface->ApplyGameplayEffectToSelf(DamageEffect);
				if(!damageEffectHandle.IsValid())
				{
					UE_LOG(LogTemp, Error, TEXT("AProjectileBase::OnHit gameplay effect Damage couldn't be applied"));
				}
				
				/*if(const auto shooterCharacter = Cast<IICharacter>(GetInstigator()))
				{

					const auto localRole = GetInstigator()->GetLocalRole();
					UE_LOG(LogTemp, Warning, TEXT("AProjectileBase::OnHit instigator local role = %d"), localRole);
					const auto otherLocalRole = OtherActor->GetLocalRole();
					UE_LOG(LogTemp, Warning, TEXT("AProjectileBase::OnHit hurt character local role = %d"), otherLocalRole);
					
					const auto gameplayAbilityInterface = shooterCharacter->GetAbilitySystemComponentBase();
					gameplayAbilityInterface->ApplyGameplayEffectToTarget(DamageEffect, hurtCharacter);
				}*/
			}
			
			/*
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
				DamageType);*/
		}
		Destroy();
	}
}
