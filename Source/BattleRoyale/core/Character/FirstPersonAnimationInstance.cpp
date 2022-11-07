// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonAnimationInstance.h"
#include "KismetAnimationLibrary.h"

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
	if(!Character)
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

	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Green, FString::Printf(TEXT("Is Aiming = %d"), IsAiming));
}

void UFirstPersonAnimationInstance::SetupCharacter()
{
	Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if(Character && Character->Implements<UICharacter>())
	{
		CharacterInterface = Character;
	}
}