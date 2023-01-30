// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"


#include "BattleRoyale/core/Weapons/ProjectileBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "AbilitySystemComponent.h"
#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Abilities/GameplayAbilityBase.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameMode.h"
#include "Components/CombatComponent.h"
#include "Components/HurtComponent.h"
#include "Components/PickupComponent.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

//////////////////////////////////////////////////////////////////////////
// ABattleRoyaleCharacter

ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;
	
	//The Character has a Mesh by default so, this mesh will be the 3rd person mesh
	GetMesh()->SetOwnerNoSee(true);	
	
	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);

	//Create hurtComponent
	HurtComponent = CreateDefaultSubobject<UHurtComponent>(TEXT("HurtComponent"));
	HurtComponent->SetIsReplicated(true);

	//Create CombatComponent
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);

	//Create PickupComponent
	PickupComponent = CreateDefaultSubobject<UPickupComponent>(TEXT("PickupComponent"));
}

void ACharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	UE_LOG(LogCharacter, Log, TEXT("ACharacterBase::BeginPlay"));
}

void ACharacterBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogCharacter, Log, TEXT("ACharacterBase::PossessedBy"));
	//Only Server
	//MulticastSpawnWeapon();
	InitializeGAS();
	GiveAbilitiesServer();
	Initialize(IsLocallyControlled());
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//only for clients
	InitializeGAS();
	BindAbilityActivationToInputComponent();
	Initialize(IsLocallyControlled());
}

void ACharacterBase::Initialize(bool isLocallyControlled)
{
	//EquipWeapon(mEquipedWeapon);
	GetCharacterMovement()->MaxWalkSpeed = GetMinWalkSpeed();
	DoInitialize(isLocallyControlled);
}

void ACharacterBase::InitializeGAS()
{
	const IIPlayerState* playerState = GetPlayerStateInterface();
	if (playerState)
	{
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
		const auto serverOrClient = HasAuthority() ? FString("Server") : FString("Client");
		const auto locallyControlled = IsLocallyControlled() ? FString("Player") : FString("Simulated");
		UE_LOG(LogTemp, Display, TEXT("[%s][%s] Character name: %s"), *serverOrClient, *locallyControlled, *GetName());
		
		InitializeAttributes();
	}
}

void ACharacterBase::InitializeAttributes() const
{
	HurtComponent->InitializeServer();
	
	const IIPlayerState* playerState = GetPlayerStateInterface();
	if (playerState)
	{
		const auto abilitySystemComponent = playerState->GetAbilitySystemComponent();
		
		if(abilitySystemComponent && DefaultAttributeEffect)
		{
			FGameplayEffectContextHandle EffectContext = abilitySystemComponent->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			const FGameplayEffectSpecHandle SpecHandle = abilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);

			if(SpecHandle.IsValid())
			{				
				//This apply works although the handle it returns is not valid
				abilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
			}
		}
	}
}
	
IIPlayerState* ACharacterBase::GetPlayerStateInterface() const
{
	const auto playerState = GetPlayerState();
	if(playerState != nullptr && playerState->Implements<UIPlayerState>())
	{
		return Cast<IIPlayerState>(playerState);
	}
	return nullptr;
}

bool ACharacterBase::IsCharacterValid() const
{
	return IsValid(this) && HurtComponent->IsReady();
}

FVector ACharacterBase::GetCurrentMeshSpaceVelocity() const
{
	return GetMesh()->GetComponentTransform().InverseTransformVector(GetVelocity());
}

float ACharacterBase::GetCurrentHealth() const
{
	return HurtComponent->GetCurrentHealth();
}

void ACharacterBase::SetInvulnerableServer(bool isInvulnerable)
{
	HurtComponent->SetInvulnerableServer(isInvulnerable);
}

//////////////////////////////////////////////////////////////////////////
// Input
// Let the input binding that refer to the character specific into the character itself.
// we can bind more other kind of binding in the Playercontroller.
void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);
	
	// Bind fire event
	//PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::OnFire);
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACharacterBase::OnResetVR);
	PlayerInputComponent->BindAction("Invulnerable", IE_Pressed, this, &ACharacterBase::OnSetInvulnerable);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ACharacterBase::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ACharacterBase::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ACharacterBase::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &ACharacterBase::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ACharacterBase::LookUpAtRate);
	
	BindAbilityActivationToInputComponent();
}

void ACharacterBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACharacterBase, mControlRotation);
}

void ACharacterBase::BindAbilityActivationToInputComponent() const
{
	const auto playerState = GetPlayerStateInterface();
	if(playerState != nullptr && playerState->GetAbilitySystemComponent() && InputComponent)
	{
		const FGameplayAbilityInputBinds Binds(
			"Confirm",
			"Cancel",
			"EAbilityInputID",
			static_cast<int32>(EAbilityInputID::Confirm),
			static_cast<int32>(EAbilityInputID::Cancel)
		);
		playerState->GetAbilitySystemComponent()->BindAbilityActivationToInputComponent(InputComponent, Binds);
	}
}

//Only server can give abilities.
void ACharacterBase::GiveAbilitiesServer()
{
	if(!HasAuthority())
	{
		return;
	}
	
	const auto playerState = GetPlayerStateInterface();
	if(playerState)
	{
		const auto abilitySystemComponent = playerState->GetAbilitySystemComponent();
		if(abilitySystemComponent)
		{
			//Asign abilities to the ability system component. As Ability system component is replicated, we are
			//asigning also to the clients.
			for(TSubclassOf<UGameplayAbilityBase>& startupAbility : mDefaultAbilities)
			{
				abilitySystemComponent->GiveAbility(
					FGameplayAbilitySpec(startupAbility, 1, static_cast<int32>(startupAbility.GetDefaultObject()->AbilityInputID), this)
				);
			}
		}
	}	
}

void ACharacterBase::SetCurrentHealthTest(float health)
{
	//Send effect to modify the health
}

bool ACharacterBase::CanSprint() const
{
	//We are not checking if the Velocity > 0 to sprint because
	//if the shift is pressed and then the player starts walking (velocity >0) then
	//the ability is not activated.
	return GetCharacterMovement()->IsWalking();
}

void ACharacterBase::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = GetMaxWalkSpeed();
	BP_OnStartSprinting(GetMaxWalkSpeed());
}

void ACharacterBase::SetEnableInput(bool enable, const FInputModeDataBase& inputMode)
{
	const auto playerController = Cast<APlayerController>(GetController());
	playerController->SetInputMode(inputMode);
	
	if(enable)
	{
		EnableInput(playerController);
	}
	else
	{
		DisableInput(playerController);
	}
}

void ACharacterBase::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = GetMinWalkSpeed();
	BP_OnStopSprinting(GetMinWalkSpeed());
}

bool ACharacterBase::CanJump() const
{
	//The reasonable will be to call Super::CanJump()
	//but seems that is returning false for testing, while putting the exact same
	//code instead is working fine.
	//return CanJump();

	
	// Ensure the character isn't currently crouched.
	bool bCanJump = !bIsCrouched;

	// Ensure that the CharacterMovement state is valid
	bCanJump &= GetCharacterMovement()->CanAttemptJump();

	if (bCanJump)
	{
		// Ensure JumpHoldTime and JumpCount are valid.
		if (!bWasJumping || GetJumpMaxHoldTime() <= 0.0f)
		{
			if (JumpCurrentCount == 0 && GetCharacterMovement()->IsFalling())
			{
				bCanJump = JumpCurrentCount + 1 < JumpMaxCount;
			}
			else
			{
				bCanJump = JumpCurrentCount < JumpMaxCount;
			}
		}
	}

	return bCanJump;
}

void ACharacterBase::StartJumping()
{
	Jump();
}

void ACharacterBase::StopJumping_()
{
	Super::StopJumping();
}

bool ACharacterBase::CanCrouch_() const
{
	return Super::CanCrouch();
}

void ACharacterBase::StartCrouching()
{
	Crouch();
}

void ACharacterBase::StopCrouching()
{
	UnCrouch();
}

bool ACharacterBase::CanShoot() const
{
	return IsAlive() && CombatComponent->CanShoot();
}

void ACharacterBase::Shoot()
{
	check(CombatComponent && CombatComponent->HasWeaponEquipped());
	
	//Character OnShoot, for instance to make a camera shake
	//We want the camera shake be only executed locally on the player who shot.
	BP_OnShoot();
	CombatComponent->Shoot();
}

IAbilitySystemInterface* ACharacterBase::GetAbilitySystemComponent() const
{
	return GetPlayerStateInterface();
}

IIAbilitySystemInterfaceBase* ACharacterBase::GetAbilitySystemComponentBase() const
{
	if(!GetPlayerStateInterface())
	{
		return nullptr;
	}
	return GetPlayerStateInterface()->GetAbilitySystemComponentInterface();		
}

IIGameMode* ACharacterBase::GetGameModeServer() const
{
	return Cast<IIGameMode>(GetWorld()->GetAuthGameMode<ABattleRoyaleGameMode>());
}

UCameraComponent* ACharacterBase::GetCamera() const
{
	TArray<UCameraComponent*> cameras;
	GetComponents<UCameraComponent>(cameras);

	if(cameras.Num() > 0)
	{
		return cameras[0];
	}
	return nullptr;
}

void ACharacterBase::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ACharacterBase::OnSetInvulnerable()
{
	mIsInvulnerable = !mIsInvulnerable;
	HurtComponent->SetInvulnerableServer(mIsInvulnerable);
}

void ACharacterBase::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ACharacterBase::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ACharacterBase::AddControllerPitchInput(float Rate)
{
	Super::AddControllerPitchInput(Rate);
	
	if(IsLocallyControlled())
	{
		//const auto rotation = mWeaponMesh->GetComponentRotation();
		ServerSetCharacterControlRotation(GetControlRotation());
	}	
}

void ACharacterBase::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ACharacterBase::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ACharacterBase::EquipServer(TScriptInterface<IPickupObject> pickableObject)
{
	if(!HasAuthority())
	{
		return false;
	}
	
	check(pickableObject);
	if(pickableObject->IsEquipped())
	{
		return false;
	}

	if(pickableObject.GetObject()->Implements<UWeapon>())
	{
		return EquipWeaponServer(pickableObject);
	}
	else
	{
		//TODO guardar en el inventario
	}
	return false;
}

bool ACharacterBase::UnEquipServer() const
{
	if(!HasAuthority())
	{
		return false;
	}
	
	const TScriptInterface<IPickupObject> pickupObject = CombatComponent->GetEquippedWeapon().GetObject();
	check(pickupObject.GetObject());
	
	pickupObject->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	
	//TODO se tiene que cambiar el estado del objeto y hacer un drop
	check(CombatComponent);
	if(CombatComponent->UnEquipWeapon())
	{
		if(IsLocallyControlled())
		{
			const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
			gameInstance->GetEventDispatcher()->OnUnEquippedWeapon.Broadcast();
		}
		return true;
	}
	return false;
}

void ACharacterBase::SetPickupObject(TScriptInterface<IPickupObject> object)
{
	check(PickupComponent);
	PickupComponent->SetPickupObject(object);
}

TScriptInterface<IPickupObject> ACharacterBase::GetPickupObject() const
{
	check(PickupComponent);
	return PickupComponent->GetPickupObject();
}

bool ACharacterBase::EquipWeaponServer(TScriptInterface<IPickupObject> pickableObject) const
{
	//TODO si ya tengo un arma hacer un unequip primero
	//equipar arma.
	
	const auto isAttached = pickableObject->AttachToComponent(
				GetMesh(),
				FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
				RightHandSocketName);
	if(!isAttached)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::Equip] pickup object not attached to the character"), *GetName());
		return false;
	}

	const TScriptInterface<IWeapon> weapon =pickableObject.GetObject();
	check(CombatComponent);
	if(CombatComponent->EquipWeapon(weapon, RightHandSocketName))
	{
		pickableObject->ChangeStateServer(EPickupObjectState::Equipped);
		
		//TODO creo que este código se puede generalizar a cualquier tipo de objeto y
		//así dejarlo aquí pero fuera del if, si se equipa cualquier cosa se envía a la ui
		//y la ui decide si es un weapon mostrar lo que quiera
		//No está mal dejar ambos eventos uno específico para armas otro para los demás objetos
		if(IsLocallyControlled())
		{
			const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
			gameInstance->GetEventDispatcher()->OnEquippedWeapon.Broadcast(CombatComponent->GetEquippedWeapon());
		}
		return true;
	}
	return false;
}

void ACharacterBase::PlayMontage(UAnimMontage* montage, USkeletalMeshComponent* mesh) const
{
	if (montage != nullptr)
	{
		UAnimInstance* AnimInstance = mesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(montage, 1.f);
		}
	}
}

void ACharacterBase::UpdateHealth(const FTakeDamageData& damage)
{
	/*if (mCurrentHealth <= 0)
	{
		UnEquipWeapon();
		HideFirstPersonMesh();
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
		if(IsLocallyControlled())
		{
			DisableInput(Cast<APlayerController>(GetController()));
			
			const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
			gameInstance->GetEventDispatcher()->OnPlayerDead.Broadcast();
		}		
	}*/
}

void ACharacterBase::DieClient()
{
	UnEquipServer();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	if(IsLocallyControlled())
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}

	DoDieClient();
}

void ACharacterBase::NotifyTakeDamage(float damage, const AActor* causer, float currentHealth)
{
	MulticastTakeDamage(damage, causer, currentHealth);
}

void ACharacterBase::DieServer()
{
	SetCanBeDamaged(false); //replicated
	GetCharacterMovement()->StopMovementImmediately();
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->SetComponentTickEnabled(false);
	
	DieClient();
}

void ACharacterBase::NotifyRefreshHealth(float health) const
{
	GetPlayerStateInterface()->NotifyRefreshHealth(health);
}

void ACharacterBase::MulticastTakeDamage_Implementation(float damage, const AActor* causer, float currentHealth)
{
	const bool isNotLocallyControlledAndCauserIsAutonomousProxy = !IsLocallyControlled() && causer && causer->GetLocalRole() == ROLE_AutonomousProxy;
	const bool isAuthorityAndCauserIsLocallyControlled = HasAuthority() && Cast<ACharacter>(causer)->IsLocallyControlled();
	if(isNotLocallyControlledAndCauserIsAutonomousProxy || isAuthorityAndCauserIsLocallyControlled)
	{
		BP_OnTakenDamage(damage, causer->GetActorLocation(), currentHealth);
		if(currentHealth<= 0.0f)
		{
			BP_OnDead();
		}
	}
}

void ACharacterBase::ServerSetCharacterControlRotation_Implementation(const FRotator& rotation)
{
	mControlRotation = rotation;
	ForceNetUpdate();
}

bool ACharacterBase::ServerSetCharacterControlRotation_Validate(const FRotator& rotation)
{
	return true;
}