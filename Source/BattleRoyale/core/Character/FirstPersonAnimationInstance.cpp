// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonAnimationInstance.h"
#include "KismetAnimationLibrary.h"
#include "Components/CombatComponent.h"
#include "Kismet/KismetMathLibrary.h"

void UFirstPersonAnimationInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	
	SetupCharacter();
	if(Character)
	{
		mStartingYawRotator = FRotator(0.0f, Character->GetBaseAimRotation().Yaw ,0.0f);
	}
}

void UFirstPersonAnimationInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if(!Character)
	{
		SetupCharacter();
	}
	check(Character);
	
	const auto gunComponent = CharacterInterface->GetGunComponent();
	check(gunComponent);
	
	if(!Character || !Character->IsCharacterValid())
	{
		return;
	}

	const auto velocity = CharacterInterface->GetCurrentVelocity();
	Speed = velocity.Length();
	Direction = UKismetAnimationLibrary::CalculateDirection(velocity, CharacterInterface->GetCurrentRotation());
	ControlRotation = CharacterInterface->GetCurrentControlRotation();	
	MeshSpaceVelocity = CharacterInterface->GetCurrentMeshSpaceVelocity();
	
	HasWeaponEquipped = gunComponent->HasWeaponEquipped();
	IsAiming = gunComponent->IsAiming();
	
	IsCrouching = CharacterInterface->IsCrouching();
	IsDead = !CharacterInterface->IsAlive();
	IsInAir = CharacterInterface->IsFalling();
	
	//This two rotators are synchronized, server, autonomous and simulated.
	const auto aimRotation = Character->GetBaseAimRotation();
	const auto movementRotation = UKismetMathLibrary::MakeRotFromX(Character->GetVelocity());
	YawOffset = UKismetMathLibrary::NormalizedDeltaRotator(movementRotation, aimRotation).Yaw;

	CheckEquippedToMakeLeftHandHoldsWeapon();
	CheckToEnableTurnInPlace();
}

void UFirstPersonAnimationInstance::SetupCharacter()
{
	Character = Cast<ACharacterBase>(TryGetPawnOwner());
	if(Character && Character->Implements<UICharacter>())
	{
		CharacterInterface = Character;
	}
}

void UFirstPersonAnimationInstance::CheckEquippedToMakeLeftHandHoldsWeapon()
{
	const auto gunComponent = CharacterInterface->GetGunComponent();
	if(gunComponent->HasWeaponEquipped())
	{
		//This is to make left hand holds the weapon with IK 
		Character->CombatComponent->SetupLeftHandSocketTransform(Character);
		LeftHandSocketTransform = gunComponent->GetEquippedWeapon()->GetLeftHandSocketTransform();
	}
}

void UFirstPersonAnimationInstance::CheckToEnableTurnInPlace()
{
	//const auto turning = GetInputAxisValue(FName("Turn"));
	const auto currentYawRotator = FRotator(0.0f, Character->GetBaseAimRotation().Yaw ,0.0f);
	
	if(Character->GetCurrentVelocity().Size() > 0.0f || Character->IsFalling() || FMath::IsNearlyZero(FMath::Abs(mLastYawRotator.Yaw - currentYawRotator.Yaw)))
	{
		TurningInPlace = ETurningInPlace::NotTurning;
		mStartingYawRotator = FRotator(0.0f, Character->GetBaseAimRotation().Yaw ,0.0f);
		mLastYawRotator = mStartingYawRotator;
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("STOP TURNING"));
		
		return;
	}

	const auto deltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(currentYawRotator, mStartingYawRotator).Yaw;

	//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("yaw rotation = %f control rotation = %f, aim rotation = %f"), Character->GetActorRotation().Yaw, Character->GetControlRotation().Yaw, Character->GetBaseAimRotation().Yaw));
	
	if(deltaYaw < 0.0f && TurningInPlace != ETurningInPlace::TurnLeft)
	{
		TurningInPlace = ETurningInPlace::TurnLeft;
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("TURNING LEFT"));
		
	}
	else if(deltaYaw > 0.0f && TurningInPlace != ETurningInPlace::TurnRight)
	{
		TurningInPlace = ETurningInPlace::TurnRight;
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("TURNING RIGHT"));
		
	}
	mLastYawRotator = currentYawRotator;
}
