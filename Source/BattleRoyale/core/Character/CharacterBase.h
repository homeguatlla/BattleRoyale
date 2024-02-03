// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Attributes/AttributeSetHealth.h"
#include "ICharacter.h"
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "Components/HurtComponent.h"
#include "Components/IGunComponent.h"
#include "Components/FootstepsComponent.h"
#include "Components/CombatComponent.h"
#include "Components/InventoryComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterBase.generated.h"

class UPickupSelectorComponent;
class IWeapon;
class IIPlayerState;
class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class BATTLEROYALE_API ACharacterBase : public ACharacter, public IICharacter
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FName RightHandSocketName;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FName RightFootSocketName;

	UPROPERTY(EditDefaultsOnly, Category = Character)
	FName LeftFootSocketName;
	
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	TArray<TSubclassOf<class UGameplayAbilityBase>> mDefaultAbilities;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "GAS Effects", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;
	
	/** The player's maximum health. This is the highest that their health can be, and the value that their health starts at when spawned.*/
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float MaxHealth{100.0};

	/** The player's maximum walk speed.*/
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float MaxWalkSpeed{400.0};

	/** The player's minimum walk speed (when aiming for instance).*/
	UPROPERTY(EditDefaultsOnly, Category = "Character")
	float MinWalkSpeed{150.0};
	
	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera, meta = (AllowPrivateAccess = "true"))
	float BaseLookUpRate;
	
	/** Replicated control rotation in order to update remotes pitch */
	UPROPERTY(Transient, Replicated)
	FRotator mControlRotation;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UHurtComponent* HurtComponent = nullptr;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	UCombatComponent* CombatComponent = nullptr;

	UPROPERTY(VisibleAnywhere)
	UFootstepsComponent* FootstepsComponent = nullptr;
	
	UPROPERTY(Replicated, VisibleAnywhere)
	UInventoryComponent* InventoryComponent = nullptr;

	UPROPERTY(Replicated, VisibleAnywhere)
	UPickupSelectorComponent* PickupSelectorComponent = nullptr;
	
	bool mIsInvulnerable = false;

public:
	
	ACharacterBase();
	
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(BlueprintCallable)
	virtual FVector GetLocation() const override { return GetActorLocation(); }
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsCharacterValid() const override;
	
	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentVelocity() const override { return GetVelocity(); }

	UFUNCTION(BlueprintCallable)
	virtual FVector GetCurrentMeshSpaceVelocity() const override;
	
	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentRotation() const override { return GetActorRotation(); }

	UFUNCTION(BlueprintCallable)
	virtual FRotator GetCurrentControlRotation() const override { return mControlRotation; }

	UFUNCTION(BlueprintCallable)
	virtual float GetCurrentHealth() const override;

	UFUNCTION(BlueprintCallable)
	virtual void SetInvulnerableServer(bool isInvulnerable) override;
	
	UFUNCTION(BlueprintCallable)
	virtual float GetMaxHealth() const override { return MaxHealth; }

	UFUNCTION(BlueprintCallable)
	virtual float GetMaxWalkSpeed() const override { return MaxWalkSpeed; }

	UFUNCTION(BlueprintCallable)
	virtual float GetMinWalkSpeed() const override { return MinWalkSpeed; }

	UFUNCTION(BlueprintCallable)
	virtual void SetMaxSpeed(float maxSpeed) override { GetCharacterMovement()->MaxWalkSpeed = maxSpeed; }
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsAlive() const override { return HurtComponent->IsAlive(); }
	
	UFUNCTION(BlueprintCallable)
	virtual bool IsFalling() const override { return GetCharacterMovement()->IsFalling(); }

	UFUNCTION(BlueprintCallable)
	virtual FName GetRightHandSocketName() const { return RightHandSocketName; }

	UFUNCTION(BlueprintCallable)
	virtual FTransform GetRightHandSocketTransform() const override;

	UFUNCTION(BlueprintCallable)
	virtual FName GetRightFootSocketName() const override { return RightFootSocketName; }

	UFUNCTION(BlueprintCallable)
	virtual FTransform GetRightFootSocketTransform() const override;
	
	UFUNCTION(BlueprintCallable)
	virtual FName GetLeftFootSocketName() const override { return LeftFootSocketName; }

	UFUNCTION(BlueprintCallable)
	virtual FTransform GetLeftFootSocketTransform() const override;
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanSprint() const override;

	UFUNCTION(BlueprintCallable)
	virtual void StartSprinting() override;

	UFUNCTION(BlueprintCallable)
	virtual UCameraComponent* GetCamera() const override;

	UFUNCTION(BlueprintCallable)
	virtual UHurtComponent* GetHurtComponent() const override { return HurtComponent; }
	
	virtual void SetEnableInput(bool enable, const FInputModeDataBase& inputMode = FInputModeGameAndUI()) override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnStartSprinting))
	void BP_OnStartSprinting(float maxSpeed);

	UFUNCTION(BlueprintCallable)
	virtual void StopSprinting() override;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnStopSprinting))
	void BP_OnStopSprinting(float maxSpeed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnShoot))
	void BP_OnShoot(bool isAiming);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnTakenDamage))
	void BP_OnTakenDamage(float damage, const FVector& damageCauserLocation, float currentHealth, bool hasToShowDamagePoints);

	UFUNCTION(BlueprintImplementableEvent, Category = "Character", meta = (DisplayName = OnDead))
	void BP_OnDead();
	
	//UFUNCTION(BlueprintCallable)
	virtual bool CanJump() const override;

	UFUNCTION(BlueprintCallable)
	virtual void StartJumping() override;

	UFUNCTION(BlueprintCallable)
	virtual void StopJumping_() override;

	virtual bool CanCrouch_() const override;
	virtual void StartCrouching() override;
	virtual void StopCrouching() override;
	UFUNCTION(BlueprintCallable)
	virtual bool IsCrouching() const override { return bIsCrouched; }
	
	UFUNCTION(BlueprintCallable)
	virtual bool CanShoot() const override;
	virtual void Shoot() override;
	
	virtual void DieServer() override;
	virtual void DieClient() override;
	
	virtual void NotifyRefreshHealth(float health) const override;
	virtual void NotifyTakeDamage(float damage, const AActor* causer, float currentHealth) override;
	
	virtual TScriptInterface<IGunComponent> GetGunComponent() const override { return CombatComponent; }
	virtual TScriptInterface<IIFootstepsComponent> GetFootstepsComponent() const override { return FootstepsComponent; }
	virtual TScriptInterface<IIInventoryComponent> GetInventoryComponent() const override { return InventoryComponent; }
	virtual UPickupSelectorComponent* GetPickupSelectorComponent() const override { return PickupSelectorComponent; }
	
	virtual IAbilitySystemInterface* GetAbilitySystemComponent() const override;
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentBase() const override;

	virtual ELifetimeCondition AllowActorComponentToReplicate(const UActorComponent* ComponentToReplicate) const override;

	//virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	//                         AActor* DamageCauser) override;
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

protected:
	
	/** Resets HMD orientation and position in VR. */
	void OnSetInvulnerable();
	void OnShowInventory();
	void OnSpeedChanged(const FOnAttributeChangeData& data);

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
	virtual void SetCurrentHealthTest(float health) override;
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void Initialize(bool isLocallyControlled);
	virtual void DoInitialize(bool isLocallyControlled) {}
	
	void InitializeGAS();
	void InitializeAttributes() const;
	//void OnHealthChanged(const FOnAttributeChangeData& OnAttributeChangeData) const;
	void BindAbilityActivationToInputComponent() const;
	void GiveAbilitiesServer();

	IIPlayerState* GetPlayerStateInterface() const;
	
	//void SpawnWeapon();
	
	void PlayMontage(UAnimMontage* montage, USkeletalMeshComponent* mesh) const;
	//void UpdateHealth(const FTakeDamageData& damage);

	bool RegisterToSpeedAttributeDelegate(std::function<void(const FOnAttributeChangeData& data)> callback) const;
	
	IIGameMode* GetGameModeServer() const;

	virtual void DoDieClient() {}
	
	virtual USkeletalMeshComponent* GetCurrentMesh() const { return GetMesh(); }

	//bool EquipWeaponServer(TScriptInterface<IPickupObject> pickableObject) const;
	
	//UFUNCTION(NetMulticast, Unreliable)
	//void MulticastSpawnWeapon();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastTakeDamage(float damage, const AActor* causer, float currentHealth);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSetMaxSpeed(float speed);
	
	UFUNCTION(Unreliable, Server, WithValidation)
	void ServerSetCharacterControlRotation(const FRotator& rotation);

	void DisableMovement() const;
	void DisableCollision() const;
	
	//UFUNCTION()
	//void OnRep_TakeDamageData();
};

