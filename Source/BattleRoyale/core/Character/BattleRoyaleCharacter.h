// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ICharacter.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BattleRoyaleCharacter.generated.h"

class IIPlayerState;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ABattleRoyaleCharacter : public ACharacter, public IICharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* mCharacterMesh1P;

	/** Character mesh: 3rd person view (seen only by others) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* mCharacterMesh3P;
	
	/** Weapon mesh */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	USkeletalMeshComponent* mWeaponMesh;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* mFirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;
	
public:
	ABattleRoyaleCharacter();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
	
	UFUNCTION(BlueprintCallable)
	virtual USkeletalMeshComponent* GetWeaponMesh() const override { return mWeaponMesh; }

	UFUNCTION(BlueprintCallable)
	virtual bool IsCharacterValid() const override { return IsValid(this); }

	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentVelocity() const override { return GetVelocity(); }
	
	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentRotation() const override { return GetActorRotation(); }

	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentControlRotation() const override { return mControlRotation; }
			
	UFUNCTION(BlueprintCallable)
	virtual bool IsFalling() const override { return GetCharacterMovement()->IsFalling(); }

protected:
	virtual void BeginPlay();

public:
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;
	
	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class ABattleRoyaleProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation1P;

	/** AnimMontage to play each time we fire third person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation3P;

	/** Replicated control rotation in order to update remotes pitch */
	UPROPERTY(Transient, Replicated)
	struct FRotator mControlRotation;
	
protected:
		
	/** Fires a projectile. */
	void OnFire();

	/** Resets HMD orientation and position in VR. */
	void OnResetVR();

	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);

	virtual void AddControllerPitchInput(float Val) override;

	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	
	// End of APawn interface
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void Initialize(bool isLocallyControlled);
	void InitializeGAS();

	IIPlayerState* GetPlayerStateInterface() const;
	
	void SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const;
	void StartRunning();
	void StopRunning();
	void FillWithWeaponMuzzleLocationAndRotation(const USkeletalMeshComponent* weapon, FVector& location, FRotator& rotation) const;
	void EquipWeapon(USkeletalMeshComponent* mesh, USkeletalMeshComponent* weapon);
	void PlayMontage(UAnimMontage* montage, USkeletalMeshComponent* mesh) const;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation);

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnFire();

	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerSetCharacterControlRotation(const FRotator& rotation);
	
public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return mCharacterMesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return mFirstPersonCameraComponent; }

};

