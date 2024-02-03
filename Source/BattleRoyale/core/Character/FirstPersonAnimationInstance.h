// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "CharacterTypes.h"
#include "Animation/AnimInstance.h"
#include "FirstPersonAnimationInstance.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UFirstPersonAnimationInstance : public UAnimInstance
{
	GENERATED_BODY()

public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
private:
	void SetupCharacter();
	void CheckEquippedToMakeLeftHandHoldsWeapon();
	void CheckEquippedToMakeWeaponAimsToCrosshair();
	void CheckToEnableTurnInPlace();
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	class ACharacterBase* Character = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	TScriptInterface<class IICharacter> CharacterInterface;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	float Speed;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	float Direction;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	FRotator ControlRotation;
	
	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	FVector MeshSpaceVelocity;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool IsInAir;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool IsAiming;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool IsCrouching;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool IsDead;

	UPROPERTY(BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = true))
	bool HasWeaponEquipped;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta = (AllowPrivateAccess = true))
	float YawOffset;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta = (AllowPrivateAccess = true))
	FTransform LeftHandSocketTransform;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta = (AllowPrivateAccess = true))
	ETurningInPlace TurningInPlace = ETurningInPlace::NotTurning;
	
	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta = (AllowPrivateAccess = true))
	FRotator RightHandRotation;

	UPROPERTY(BlueprintReadOnly, Category= "Movement", meta = (AllowPrivateAccess = true))
	bool IsLocallyControlled = false;
	
	FRotator mStartingYawRotator;
	FRotator mLastYawRotator;
};
