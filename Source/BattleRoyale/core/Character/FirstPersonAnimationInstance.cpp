// Fill out your copyright notice in the Description page of Project Settings.


#include "FirstPersonAnimationInstance.h"

#include "KismetAnimationLibrary.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
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
	
	const auto gunComponent = CharacterInterface->GetGunComponent();
	check(gunComponent);
	
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
	CheckEquippedToMakeWeaponPointsToCrosshair();
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

void UFirstPersonAnimationInstance::CheckEquippedToMakeWeaponPointsToCrosshair()
{
	if(!Character->IsLocallyControlled())
	{
		return;
	}

	IsLocallyControlled = true;
	
	const auto gunComponent = CharacterInterface->GetGunComponent();
	if(gunComponent->HasWeaponEquipped())
	{
		const auto rightHandSocketTransform = CharacterInterface->GetRightHandSocketTransform();
		const auto shootingTarget = gunComponent->GetShootingTargetLocation();

		//En este punto queremos que el arma apunte en la dirección de la retícula.
		//Para ello tenemos que el eje que queremos mover es el eje Y del arma. Hay que usar el MakeRotFromY pasando el nuevo vector a donde queremos calcular la rotación para posicionarlo
		//el vector es el vector que va desde la mano, hasta el shootingTarget (el punto en el espacio de la reticula)
		//vector = shootingTarget - rightHandSocketTransform.GetLocation()
		//Como el vector Y está apuntando no hacia la dirección donde apunta el arma sino hacia el lado opuesto, calculamos la posición del target como si estuviera en la dirección opuesta.
		//newTargetLocation = rhst.GetLocation() + rhst.GetLocation-shootingTarget y luego el vector
		//newTargetLocation - rhst.GetLocation()
		const auto newShootingTargetLocation = (rightHandSocketTransform.GetLocation() + rightHandSocketTransform.GetLocation() - shootingTarget);
		RightHandRotation = UKismetMathLibrary::MakeRotFromY( newShootingTargetLocation - rightHandSocketTransform.GetLocation());
	
		//Weapon axis
		/*DrawDebugLine(GetWorld(), rightHandSocketTransform.GetLocation(), rightHandSocketTransform.GetLocation() +  rightHandSocketTransform.GetRotation().GetForwardVector() * 100.0f, FColor::Red);
		DrawDebugLine(GetWorld(), rightHandSocketTransform.GetLocation(), rightHandSocketTransform.GetLocation() +  rightHandSocketTransform.GetRotation().GetAxisY() * 100.0f, FColor::Green);//, false, -1, 0, 2);
		DrawDebugLine(GetWorld(), rightHandSocketTransform.GetLocation(), rightHandSocketTransform.GetLocation() +  rightHandSocketTransform.GetRotation().GetUpVector() * 100.0f, FColor::Blue);
		*/
		/*const auto weapon = gunComponent->GetEquippedWeapon();
		const auto muzzleSocketTransform = weapon->GetMuzzleSocketTransform();
		DrawDebugLine(GetWorld(), muzzleSocketTransform.GetLocation(), muzzleSocketTransform.GetLocation() +  muzzleSocketTransform.GetRotation().GetForwardVector() * 10000.0f, FColor::Yellow);
		DrawDebugLine(GetWorld(), muzzleSocketTransform.GetLocation(), shootingTarget, FColor::Cyan);
		*/
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
