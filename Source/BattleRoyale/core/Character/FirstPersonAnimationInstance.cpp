// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonAnimationInstance.h"
#include "KismetAnimationLibrary.h"
#include "Kismet/KismetMathLibrary.h"

void UFirstPersonAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	SetupCharacter();
}

void UFirstPersonAnimationInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!Character)
	{
		SetupCharacter();
	}
	if(!Character || !Character->IsCharacterValid())
	{
		return;
	}

	const auto velocity = CharacterInterface->GetCurrentVelocity();
	Speed = velocity.Length();

	Direction = UKismetAnimationLibrary::CalculateDirection(velocity, CharacterInterface->GetCurrentRotation());

	ControlRotation = CharacterInterface->GetCurrentControlRotation();
	
	MeshSpaceVelocity = CharacterInterface->GetCurrentMeshSpaceVelocity();

	IsAiming = CharacterInterface->IsAiming();
	IsCrouching = CharacterInterface->IsCrouching();
	IsDead = !CharacterInterface->IsAlive();
	IsInAir = CharacterInterface->IsFalling();
	HasWeaponEquipped = CharacterInterface->HasWeaponEquipped();

	//This two rotators are synchronized, server, autonomous and simulated.
	const auto aimRotation = Character->GetBaseAimRotation();
	const auto movementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation).Yaw;
}

void UFirstPersonAnimationInstance::SetupCharacter()
{
	Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if(Character && Character->Implements<UICharacter>())
	{
		CharacterInterface = Character;
	}
}
