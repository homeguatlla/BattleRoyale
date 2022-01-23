// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"
#include "BattleRoyale/core/Weapons/ProjectileBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h" // for FXRMotionControllerBase::RightHandSourceId
#include "Engine/SkeletalMeshSocket.h"
//#include "UObject/CoreNetTypes.h"
#include "AbilitySystemComponent.h"
#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Abilities/GameplayAbilityBase.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ABattleRoyaleCharacter

ACharacterBase::ACharacterBase()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	mFirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	mFirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	mFirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	mFirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	mCharacterMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	mCharacterMesh1P->SetOnlyOwnerSee(true);
	mCharacterMesh1P->SetupAttachment(mFirstPersonCameraComponent);
	mCharacterMesh1P->bCastDynamicShadow = false;
	mCharacterMesh1P->CastShadow = false;
	mCharacterMesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	mCharacterMesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	
	mCharacterMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	mCharacterMesh3P->SetOwnerNoSee(true);
	mCharacterMesh3P->SetupAttachment(mFirstPersonCameraComponent);
	mCharacterMesh3P->bCastDynamicShadow = false;
	mCharacterMesh3P->CastShadow = false;
	
	// Create a gun mesh component
	mWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	mWeaponMesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	mWeaponMesh->bCastDynamicShadow = false;
	mWeaponMesh->CastShadow = false;
	
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);
}

void ACharacterBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	UE_LOG(LogCharacter, Log, TEXT("ABattleRoyaleCharacter::PossessedBy"));
	//Only Server
	Initialize(IsLocallyControlled());
	InitializeGAS();
	GiveAbilitiesServer();
}

void ACharacterBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//only for clients
	InitializeGAS();
	BindAbilityActivationToInputComponent();
}

void ACharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	UE_LOG(LogCharacter, Log, TEXT("ABattleRoyaleCharacter::BeginPlay"));	
	Initialize(IsLocallyControlled());
}

void ACharacterBase::Initialize(bool isLocallyControlled)
{
	EquipWeapon(isLocallyControlled ? mCharacterMesh1P: mCharacterMesh3P, mWeaponMesh);
	mCharacterMesh1P->SetHiddenInGame(!isLocallyControlled, true);
}

void ACharacterBase::InitializeGAS()
{
	IIPlayerState* playerState = GetPlayerStateInterface();
	if (playerState)
	{
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
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

//////////////////////////////////////////////////////////////////////////
// Input

void ACharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ACharacterBase::OnFire);
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ACharacterBase::OnResetVR);

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

	DOREPLIFETIME_CONDITION(ACharacterBase, mControlRotation, COND_SimulatedOnly);
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

bool ACharacterBase::CanSprint() const
{
	return GetCharacterMovement()->IsWalking();
}

void ACharacterBase::StartSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ACharacterBase::StopSprinting()
{
	GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

void ACharacterBase::OnFire()
{
	// try and fire a projectile:
	//the server has the weapon in FP1, but for the clients it has the weapons as 3P
	//so, we need when shooting send to the server our weapon location and rotation
	//because server will get wrong location and rotation for clients
	FVector muzzleLocation;
	FRotator muzzleRotation;
	FillWithWeaponMuzzleLocationAndRotation(mWeaponMesh, muzzleLocation, muzzleRotation);			
	ServerSpawnProjectile(muzzleLocation, muzzleRotation);

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	PlayMontage(FireAnimation1P, mCharacterMesh1P);
}

void ACharacterBase::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
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

void ACharacterBase::SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const
{
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// spawn the projectile at the muzzle
			World->SpawnActor<AProjectileBase>(ProjectileClass, muzzleLocation, muzzleRotation, ActorSpawnParams);
		}
	}
}

void ACharacterBase::ServerSpawnProjectile_Implementation(const FVector& muzzleLocation, const FRotator& muzzleRotation)
{
	SpawnProjectile(muzzleLocation, muzzleRotation);

	//Notify remotes about a fire in order they can play the proper animation.
	MulticastOnFire();
}

bool ACharacterBase::ServerSpawnProjectile_Validate(const FVector& muzzleLocation, const FRotator& muzzleRotation)
{
	//TODO validate there are bullets
	return true;
}

void ACharacterBase::FillWithWeaponMuzzleLocationAndRotation(const USkeletalMeshComponent* weapon, FVector& location, FRotator& rotation) const
{
	const auto weaponMuzzleSocket = weapon->GetSocketByName(TEXT("MuzzleSocket"));
	if(weaponMuzzleSocket)
	{
		location = weaponMuzzleSocket->GetSocketLocation(weapon);
		rotation = weaponMuzzleSocket->GetSocketTransform(weapon).GetRotation().Rotator();
	}
	else
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ABattleRoyaleCharacter::FillWithWeaponMuzzleLocationAndRotation] muzzle not found"),*GetName());
	}
}

void ACharacterBase::EquipWeapon(USkeletalMeshComponent* mesh, USkeletalMeshComponent* weapon)
{
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	const auto isAttached = weapon->AttachToComponent(mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHandSocket"));

	if(!isAttached)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ABattleRoyaleCharacter::EquipWeapon] weapon not attached to the character"), *GetName());	
	}
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

void ACharacterBase::ServerSetCharacterControlRotation_Implementation(const FRotator& rotation)
{
	mControlRotation = rotation;
}

bool ACharacterBase::ServerSetCharacterControlRotation_Validate(const FRotator& rotation)
{
	return true;
}

void ACharacterBase::MulticastOnFire_Implementation()
{
	if(IsLocallyControlled())
	{
		return;
	}

	PlayMontage(FireAnimation3P, mCharacterMesh3P);
}
