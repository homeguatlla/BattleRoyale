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

	CheckEquippedToMakeLeftHandHoldsWeapon();
	if(Character->IsLocallyControlled())
	{
		Character->CheckToEnableTurnInPlace();
	}
	TurningInPlace = Character->GetTurnInPlace();
	
	if(Character->HasAuthority() && !Character->IsLocallyControlled())
	{
		if(TurningInPlace == ETurningInPlace::TurnRight)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("TURNING RIGHT"));
		}
		else if(TurningInPlace == ETurningInPlace::TurnLeft)
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("TURNING LEFT"));
		}
	}
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
	if(CharacterInterface->HasWeaponEquipped())
	{
		//This is to make left hand holds the weapon with IK 
		Character->CombatComponent->SetupLeftHandSocketTransform(Character);
		LeftHandSocketTransform = Character->GetEquippedWeapon()->GetLeftHandSocketTransform();
	}
}

void UFirstPersonAnimationInstance::CheckToEnableTurnInPlace(float DeltaSeconds)
{
	const auto turning = Character->GetInputAxisValue(FName("Turn"));
	if(CharacterInterface->GetCurrentVelocity().Size() > 0.0f || CharacterInterface->IsFalling() || turning == 0.0f)
	{
		//Save the current yaw when moving or falling
		mStartingYawRotator = FRotator(0.0f, Character->GetBaseAimRotation().Yaw ,0.0f);
		mTurnInPlaceDeltaYaw = 0.0f;
		//Character->bUseControllerRotationYaw = true;
		InterpolatedTurnInPlaceDeltaYaw = 0.0f;
		mTime = 0.0f;
		TurningInPlace = ETurningInPlace::NotTurning;
		if(!Character->HasAuthority() && Character->IsLocallyControlled())
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("STOP TURNING"));
		}
		return;
	}
	//Turn in place when standing and not moving
	const auto currentYawRotator = FRotator(0.0f, Character->GetBaseAimRotation().Yaw ,0.0f);
	const auto deltaYaw = UKismetMathLibrary::NormalizedDeltaRotator(currentYawRotator, mStartingYawRotator).Yaw;
	//Character->bUseControllerRotationYaw = false;
	
	
	if(/*deltaYaw < -1*/ turning < 0.0f && TurningInPlace != ETurningInPlace::TurnLeft)
	{
		TurningInPlace = ETurningInPlace::TurnLeft;
		mTurnInPlaceDeltaYaw = deltaYaw;
		mTime = 0.0f;
		if(!Character->HasAuthority() && Character->IsLocallyControlled())
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("TURNING LEFT"));
		}
	}
	else if(/*deltaYaw > 1*/ turning > 0.0f && TurningInPlace != ETurningInPlace::TurnRight)
	{
		TurningInPlace = ETurningInPlace::TurnRight;
		mTurnInPlaceDeltaYaw = deltaYaw;
		mTime = 0.0f;
		if(!Character->HasAuthority() && Character->IsLocallyControlled())
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, TEXT("TURNING RIGHT"));
		}
	}

	if(!Character->HasAuthority() && Character->IsLocallyControlled())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("turn: %f, turn in place: %d"),turning, TurningInPlace));
	}
	
	if(TurningInPlace != ETurningInPlace::NotTurning)
	{
		//mInterpolatedTurnInPlaceDeltaYaw = FMath::FInterpTo(0.0f, mInterpolatedTurnInPlaceDeltaYaw, DeltaSeconds,  1.0f);
		//InterpolatedTurnInPlaceDeltaYaw = mTurnInPlaceDeltaYaw * mTime;
		//mTime += DeltaSeconds;
		
		/*if(!Character->HasAuthority() && Character->IsLocallyControlled())
		{
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString::Printf(TEXT("interpolated turn in place: %f, real yaw: %f time: %f"),
				InterpolatedTurnInPlaceDeltaYaw,
				Character->GetBaseAimRotation().Yaw,
				mTime));
		}*/
		//if(FMath::Abs(InterpolatedTurnInPlaceDeltaYaw)  > 75.0f)
		/*if(mTime > 1.f) //1.17f seconds is the turn 90 duration
		{
			TurningInPlace = ETurningInPlace::NotTurning;
			mStartingYawRotator = FRotator(0.0f, Character->GetBaseAimRotation().Yaw ,0.0f);
			mTurnInPlaceDeltaYaw = 0.0f;
			InterpolatedTurnInPlaceDeltaYaw = 0.0f;
			mTime = 0.0f;
			//Character->bUseControllerRotationYaw = true;
		}*/
	}
}
