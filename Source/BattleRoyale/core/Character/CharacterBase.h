// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "ICharacter.h"
#include "BattleRoyale/core/Data/TakeDamageData.h"
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/Weapons/WeaponBase.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterBase.generated.h"

class IIWeapon;
class IIPlayerState;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ACharacterBase : public ACharacter, public IICharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* mCharacterMesh1P;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* mFirstPersonCameraComponent;

	/** Motion controller (right hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* R_MotionController;

	/** Motion controller (left hand) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UMotionControllerComponent* L_MotionController;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FName RightHandSocketName;
	
	TScriptInterface<IIWeapon> mEquipedWeapon;

	float mCurrentHealth;
	
	UPROPERTY()
	UMaterialInstanceDynamic* mCharacterMesh1PMaterial = nullptr;
	
	UPROPERTY(ReplicatedUsing=OnRep_TakeDamageData)
	FTakeDamageData mDamageCauser;

	bool mAnyKeyPressed = false;

public:
	ACharacterBase();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	virtual FVector GetLocation() const override { return GetActorLocation(); }
	
	UFUNCTION(BlueprintCallable)
	virtual TScriptInterface<IIWeapon> GetEquippedWeapon() const override;

	UFUNCTION(BlueprintCallable)
	virtual bool IsCharacterValid() const override { return IsValid(this); }

	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentVelocity() const override { return GetVelocity(); }

	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentMeshSpaceVelocity() const override;
	
	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentRotation() const override { return GetActorRotation(); }

	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentControlRotation() const override { return mControlRotation; }

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() const override { return mCurrentHealth; }
	
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const override { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const override { return mCurrentHealth > 0; }
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsFalling() const override { return GetCharacterMovement()->IsFalling(); }

	UFUNCTION(BlueprintCallable)
	virtual bool CanSprint() const override;

	UFUNCTION(BlueprintCallable)
	virtual void StartSprinting() override;

	virtual void SetEnableInput(bool enable, const FInputModeDataBase& inputMode = FInputModeGameAndUI()) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnStartSprinting))
	void BP_OnStartSprinting(float maxSpeed);

	UFUNCTION(BlueprintCallable)
	virtual void StopSprinting() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnStopSprinting))
	void BP_OnStopSprinting(float maxSpeed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnShoot))
	void BP_OnShoot();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character")
	void OnTakenDamage(float damage, const FVector& damageCauserLocation, float currentHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnDead))
	void BP_OnDead();
	
	//UFUNCTION(BlueprintCallable)
	virtual bool CanJump() const override;

	UFUNCTION(BlueprintCallable)
	virtual void StartJumping() override;

	UFUNCTION(BlueprintCallable)
	virtual void StopJumping_() override;
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanShoot() const override;

	virtual void ServerShoot() override;

	virtual void Shoot() override;

	virtual void DieServer() override;

	virtual void DieClient() override;
	
	virtual UAnimMontage* GetShootingMontage() const override;

	virtual UAnimMontage* GetSimulatedShootingMontage() const override;

	virtual UAnimInstance* GetAnimationInstance() const override;

	virtual IAbilitySystemInterface* GetAbilitySystemComponent() const override;

	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentBase() const override;
	
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	                         AActor* DamageCauser) override;
	void OnAnyKeyPressed();
	void OnAnyKeyReleased();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ChangeCharacterMesh1PColor(const FColor& color);

	
protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;
	
public:
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	UPROPERTY(EditDefaultsOnly, Category = EquipedWeapon)
	TSubclassOf<class AWeaponBase> WeaponClass;
	
	/** AnimMontage to play each time we fire first person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation1P;

	/** AnimMontage to play each time we fire third person */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation3P;

	/** Replicated control rotation in order to update remotes pitch */
	UPROPERTY(Transient, Replicated)
	struct FRotator mControlRotation;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character")
	TArray<TSubclassOf<class UGameplayAbilityBase>> mDefaultAbilities;

	/** The player's maximum health. This is the highest that their health can be, and the value that their health starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float MaxHealth{100.0};

protected:
	
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

	//Only for TEST porposes
	virtual void SetCurrentHealth(float health) override;
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Initialize(bool isLocallyControlled);
	void HideFirstPersonMesh() const;
	void InitializeGAS();

	void BindAbilityActivationToInputComponent() const;
	void GiveAbilitiesServer();

	IIPlayerState* GetPlayerStateInterface() const;
	
	void SpawnWeapon();
	
	void EquipWeapon(USkeletalMeshComponent* characterMesh, TScriptInterface<IIWeapon> weapon);
	void UnEquipWeapon() const;
	void PlayMontage(UAnimMontage* montage, USkeletalMeshComponent* mesh) const;
	void UpdateHealth(const FTakeDamageData& damage);
	
	IIGameMode* GetGameModeServer() const;

	UFUNCTION(Reliable, Server, WithValidation)
	void ServerSpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation);
	
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastOnFire();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastSpawnWeapon();
	
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerSetCharacterControlRotation(const FRotator& rotation);
	
	UFUNCTION()
	void OnRep_TakeDamageData();

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return mCharacterMesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return mFirstPersonCameraComponent; }

};

