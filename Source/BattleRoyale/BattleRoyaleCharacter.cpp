// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleCharacter.h"
#include "BattleRoyaleProjectile.h"
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

DEFINE_LOG_CATEGORY_STATIC(LogCharacter, Warning, All);

//////////////////////////////////////////////////////////////////////////
// ABattleRoyaleCharacter

ABattleRoyaleCharacter::ABattleRoyaleCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	m_FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	m_FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	m_FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	m_FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	m_CharacterMesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	m_CharacterMesh1P->SetOnlyOwnerSee(true);
	m_CharacterMesh1P->SetupAttachment(m_FirstPersonCameraComponent);
	m_CharacterMesh1P->bCastDynamicShadow = false;
	m_CharacterMesh1P->CastShadow = false;
	m_CharacterMesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	m_CharacterMesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));
	
	m_CharacterMesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	m_CharacterMesh3P->SetOwnerNoSee(true);
	m_CharacterMesh3P->SetupAttachment(m_FirstPersonCameraComponent);
	m_CharacterMesh3P->bCastDynamicShadow = false;
	m_CharacterMesh3P->CastShadow = false;
	
	// Create a gun mesh component
	m_WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	m_WeaponMesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	m_WeaponMesh->bCastDynamicShadow = false;
	m_WeaponMesh->CastShadow = false;
	
	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Create VR Controllers.
	R_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("R_MotionController"));
	R_MotionController->MotionSource = FXRMotionControllerBase::RightHandSourceId;
	R_MotionController->SetupAttachment(RootComponent);
	L_MotionController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("L_MotionController"));
	L_MotionController->SetupAttachment(RootComponent);
}

void ABattleRoyaleCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	EquipWeapon(IsLocallyControlled() ? m_CharacterMesh1P: m_CharacterMesh3P, m_WeaponMesh);
	m_CharacterMesh1P->SetHiddenInGame(!IsLocallyControlled(), true);
}

//////////////////////////////////////////////////////////////////////////
// Input

void ABattleRoyaleCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind run event
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &ABattleRoyaleCharacter::StartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &ABattleRoyaleCharacter::StopRunning);
	
	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ABattleRoyaleCharacter::OnFire);

	// Enable touchscreen input
	EnableTouchscreenMovement(PlayerInputComponent);

	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &ABattleRoyaleCharacter::OnResetVR);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &ABattleRoyaleCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ABattleRoyaleCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &ABattleRoyaleCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &ABattleRoyaleCharacter::LookUpAtRate);
}

void ABattleRoyaleCharacter::StartRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
}

void ABattleRoyaleCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = 100.f;
}

void ABattleRoyaleCharacter::OnFire()
{
	// try and fire a projectile:
	//the server has the weapon in FP1, but for the clients it has the weapons as 3P
	//so, we need when shooting send to the server our weapon location and rotation
	//because server will get wrong location and rotation for clients
	FVector muzzleLocation;
	FRotator muzzleRotation;
	FillWithWeaponMuzzleLocationAndRotation(m_WeaponMesh, muzzleLocation, muzzleRotation);			
	ServerSpawnProjectile(muzzleLocation, muzzleRotation);

	// try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = m_CharacterMesh1P->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

void ABattleRoyaleCharacter::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void ABattleRoyaleCharacter::BeginTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == true)
	{
		return;
	}
	if ((FingerIndex == TouchItem.FingerIndex) && (TouchItem.bMoved == false))
	{
		OnFire();
	}
	TouchItem.bIsPressed = true;
	TouchItem.FingerIndex = FingerIndex;
	TouchItem.Location = Location;
	TouchItem.bMoved = false;
}

void ABattleRoyaleCharacter::EndTouch(const ETouchIndex::Type FingerIndex, const FVector Location)
{
	if (TouchItem.bIsPressed == false)
	{
		return;
	}
	TouchItem.bIsPressed = false;
}

//Commenting this section out to be consistent with FPS BP template.
//This allows the user to turn without using the right virtual joystick

//void ABattleRoyaleCharacter::TouchUpdate(const ETouchIndex::Type FingerIndex, const FVector Location)
//{
//	if ((TouchItem.bIsPressed == true) && (TouchItem.FingerIndex == FingerIndex))
//	{
//		if (TouchItem.bIsPressed)
//		{
//			if (GetWorld() != nullptr)
//			{
//				UGameViewportClient* ViewportClient = GetWorld()->GetGameViewport();
//				if (ViewportClient != nullptr)
//				{
//					FVector MoveDelta = Location - TouchItem.Location;
//					FVector2D ScreenSize;
//					ViewportClient->GetViewportSize(ScreenSize);
//					FVector2D ScaledDelta = FVector2D(MoveDelta.X, MoveDelta.Y) / ScreenSize;
//					if (FMath::Abs(ScaledDelta.X) >= 4.0 / ScreenSize.X)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.X * BaseTurnRate;
//						AddControllerYawInput(Value);
//					}
//					if (FMath::Abs(ScaledDelta.Y) >= 4.0 / ScreenSize.Y)
//					{
//						TouchItem.bMoved = true;
//						float Value = ScaledDelta.Y * BaseTurnRate;
//						AddControllerPitchInput(Value);
//					}
//					TouchItem.Location = Location;
//				}
//				TouchItem.Location = Location;
//			}
//		}
//	}
//}

void ABattleRoyaleCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void ABattleRoyaleCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void ABattleRoyaleCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void ABattleRoyaleCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

bool ABattleRoyaleCharacter::EnableTouchscreenMovement(class UInputComponent* PlayerInputComponent)
{
	if (FPlatformMisc::SupportsTouchInput() || GetDefault<UInputSettings>()->bUseMouseForTouch)
	{
		PlayerInputComponent->BindTouch(EInputEvent::IE_Pressed, this, &ABattleRoyaleCharacter::BeginTouch);
		PlayerInputComponent->BindTouch(EInputEvent::IE_Released, this, &ABattleRoyaleCharacter::EndTouch);

		//Commenting this out to be more consistent with FPS BP template.
		//PlayerInputComponent->BindTouch(EInputEvent::IE_Repeat, this, &ABattleRoyaleCharacter::TouchUpdate);
		return true;
	}
	
	return false;
}

void ABattleRoyaleCharacter::SpawnProjectile(const FVector& muzzleLocation, const FRotator& muzzleRotation) const
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
			World->SpawnActor<ABattleRoyaleProjectile>(ProjectileClass, muzzleLocation, muzzleRotation, ActorSpawnParams);
		}
	}
}

void ABattleRoyaleCharacter::ServerSpawnProjectile_Implementation(const FVector& muzzleLocation, const FRotator& muzzleRotation)
{
	SpawnProjectile(muzzleLocation, muzzleRotation);
}

bool ABattleRoyaleCharacter::ServerSpawnProjectile_Validate(const FVector& muzzleLocation, const FRotator& muzzleRotation)
{
	//TODO validate there are bullets
	return true;
}

void ABattleRoyaleCharacter::FillWithWeaponMuzzleLocationAndRotation(const USkeletalMeshComponent* weapon, FVector& location, FRotator& rotation) const
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

void ABattleRoyaleCharacter::EquipWeapon(USkeletalMeshComponent* mesh, USkeletalMeshComponent* weapon)
{
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	const auto isAttached = weapon->AttachToComponent(mesh, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("RightHandSocket"));

	if(!isAttached)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ABattleRoyaleCharacter::EquipWeapon] weapon not attached to the character"), *GetName());	
	}
}