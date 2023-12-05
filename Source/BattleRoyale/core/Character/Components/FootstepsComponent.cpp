// Fill out your copyright notice in the Description page of Project Settings.


#include "FootstepsComponent.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/PhysicalMaterial/PhysicalMaterialBase.h"


static TAutoConsoleVariable<bool> ConsoleShowFootsteps(
	TEXT("BR.ShowFootsteps"),
	false,
	TEXT("Draws debug info about footsteps")
	TEXT(" false: off/n")
	TEXT(" true:   on/n"),
	ECVF_Cheat);

UFootstepsComponent::UFootstepsComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFootstepsComponent::HandleFootstep(bool isRightFoot) const
{
	const auto character = Cast<IICharacter>(GetOwner());
	if(!character)
	{
		return;
	}

	const auto footLocation = isRightFoot ? character->GetRightFootSocketTransform().GetLocation() : character->GetLeftFootSocketTransform().GetLocation();
	FHitResult result;
	FCollisionQueryParams params;
	params.bReturnPhysicalMaterial = true;
	params.AddIgnoredActor(GetOwner());

	const auto rayStart = footLocation + FVector::UpVector * 20.0f;
	const auto rayEnd = footLocation - FVector::UpVector * 20.0f;
	
	FHitResult hitResult;
	if(!GetWorld()->LineTraceSingleByChannel(
		hitResult,
		rayStart,
		rayEnd,
		ECollisionChannel::ECC_Visibility,
		params))
	{
		//No collision
		return;
	}

	if(!hitResult.bBlockingHit)
	{
		return;
	}

	if(const auto physicalMaterial = hitResult.PhysMaterial.Get())
	{
		const auto material = Cast<UPhysicalMaterialBase>(physicalMaterial);
		if(!material)
		{
			return;
		}

		if(ConsoleShowFootsteps.GetValueOnAnyThread())
		{
			DrawDebugString(GetWorld(), hitResult.Location, physicalMaterial->GetName(), nullptr, FColor::White, 4.f);
			DrawDebugSphere(GetWorld(), hitResult.Location, 5, 20, FColor::Green, false, 4.0f);
		}
			
		//Play sound
		UGameplayStatics::PlaySoundAtLocation(this, material->FootstepSound, hitResult.Location, FRotator::ZeroRotator);
	}
}

void UFootstepsComponent::BeginPlay()
{
	Super::BeginPlay();
}

