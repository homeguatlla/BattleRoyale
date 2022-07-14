// Copyright Epic Games, Inc. All Rights Reserved.

#include "CharacterBase.h"


#include "BattleRoyale/core/Weapons/ProjectileBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
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
#include "Components/HurtComponent.h"
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
	
	//Create gameplayability attributes for this character
	//mGameplayAbilityAttributes = CreateDefaultSubobject<UAttributeSetBase>("GameplayAbilityAttributes");
	
	//mGameplayAbilityAttributes->InitHealth(MaxHealth);
	//mGameplayAbilityAttributes->InitMaxHealth(MaxHealth);
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
	MulticastSpawnWeapon();
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

void ACharacterBase::Initialize(bool isLocallyControlled)
{
	EquipWeapon(GetCurrentMesh(isLocallyControlled), mEquipedWeapon);
	
	DoInitialize(isLocallyControlled);
}

void ACharacterBase::InitializeGAS()
{
	const IIPlayerState* playerState = GetPlayerStateInterface();
	if (playerState)
	{
		playerState->GetAbilitySystemComponent()->InitAbilityActorInfo(GetPlayerState(), this);
		InitializeAttributes();
	}
}

void ACharacterBase::InitializeAttributes() const
{
	HurtComponent->Initialize();
		
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

TScriptInterface<IIWeapon> ACharacterBase::GetEquippedWeapon() const
{
	return mEquipedWeapon;
}

FVector ACharacterBase::GetCurrentMeshSpaceVelocity() const
{
	return GetMesh()->GetComponentTransform().InverseTransformVector(GetVelocity());
}

float ACharacterBase::GetCurrentHealth() const
{
	return GetPlayerStateInterface()->GetCurrentHealth();
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

	DOREPLIFETIME_CONDITION(ACharacterBase, mControlRotation, COND_SimulatedOnly);
	DOREPLIFETIME(ACharacterBase, mDamageCauser);
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

void ACharacterBase::SetCurrentHealth(float health)
{
	//mGameplayAbilityAttributes->SetHealth(health);
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
	GetCharacterMovement()->MaxWalkSpeed = 600.f;
	BP_OnStartSprinting(600.0f);
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
	GetCharacterMovement()->MaxWalkSpeed = 100.f;
	BP_OnStopSprinting(100.0f);
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

bool ACharacterBase::CanShoot() const
{
	const auto equippedWeapon = GetEquippedWeapon();

	return IsAlive() && equippedWeapon && equippedWeapon->CanBeFired();
}

void ACharacterBase::ServerShoot()
{
	const auto weapon = GetEquippedWeapon();
	if(weapon.GetObject() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::FillWithWeaponMuzzleLocationAndRotation] weapon is null"), *GetName());
		return;
	}
	
	//TODO los efectos no se ven en los remotos. Entonces, quizá lo que hay que hacer es, dejar el disparo en server
	//y hacer un multicast en el server para que se ejecute en los clientes y así añadir el tema efectos y sonido.
	if(HasAuthority())
	{
		weapon->Fire(weapon->GetMuzzleLocation(), weapon->GetMuzzleRotation());
	}
}

void ACharacterBase::Shoot()
{
	const auto weapon = GetEquippedWeapon();
	if(weapon.GetObject() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::FillWithWeaponMuzzleLocationAndRotation] weapon is null"), *GetName());
		return;
	}
	BP_OnShoot();
	weapon->FireClient(true);
	//We must send the weapon information because the weapon the client and server are in different positions (3P, 1P)
	ServerSpawnProjectile(weapon->GetMuzzleLocation(), weapon->GetMuzzleRotation());
}

UAnimMontage* ACharacterBase::GetShootingMontage() const
{
	if(IsLocallyControlled())
	{
		return FireAnimation1P;
	}
	else
	{
		return FireAnimation3P;
	}
}

UAnimMontage* ACharacterBase::GetSimulatedShootingMontage() const
{
	return FireAnimation3P;
}

UAnimInstance* ACharacterBase::GetAnimationInstance() const
{
	return GetMesh()->GetAnimInstance();
}

IAbilitySystemInterface* ACharacterBase::GetAbilitySystemComponent() const
{
	return GetPlayerStateInterface();
}

IIAbilitySystemInterfaceBase* ACharacterBase::GetAbilitySystemComponentBase() const
{
	return GetPlayerStateInterface()->GetAbilitySystemComponentInterface();
}

IIGameMode* ACharacterBase::GetGameModeServer() const
{
	return Cast<IIGameMode>(GetWorld()->GetAuthGameMode<ABattleRoyaleGameMode>());
}

float ACharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
                                 AActor* DamageCauser)
{
	const auto actualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if(!HasAuthority())
	{
		return actualDamage;
	}
	
	if(!EventInstigator->IsA(APlayerController::StaticClass()))
	{
		return actualDamage;
	}

	const auto killer = Cast<APlayerController>(EventInstigator);
	const auto victim = Cast<APlayerController>(GetController());

	/*const auto gameMode = GetGameModeServer();
	if(!gameMode->CanPlayerCauseDamageTo(killer, victim))
	{
		return actualDamage;
	}
*/
	//HurtComponent->TakeDamageServer(actualDamage, killer, victim);
	
	return actualDamage;
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
	m_IsInvulnerable = !m_IsInvulnerable;
	HurtComponent->SetInvulnerableServer(m_IsInvulnerable);
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

void ACharacterBase::ServerSpawnProjectile_Implementation(const FVector& muzzleLocation, const FRotator& muzzleRotation)
{
	//SpawnProjectile(muzzleLocation, muzzleRotation);
	const auto weapon = GetEquippedWeapon();
	weapon->Fire(muzzleLocation, muzzleRotation);
	//Notify all about a fire in order they can play the proper animation.
	MulticastOnFire();
}

bool ACharacterBase::ServerSpawnProjectile_Validate(const FVector& muzzleLocation, const FRotator& muzzleRotation)
{
	//TODO validate there are bullets
	return true;
}

void ACharacterBase::EquipWeapon(USkeletalMeshComponent* characterMesh, TScriptInterface<IIWeapon> weapon)
{
	if(weapon.GetObject() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::EquipWeapon] weapon is null"), *GetName());
		return;
	}
	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	const auto isAttached = weapon->AttachToComponent(
		characterMesh,
		FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true),
		RightHandSocketName);
	
	if(!isAttached)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::EquipWeapon] weapon not attached to the character"), *GetName());
		return;
	}
	
	weapon->SetCharacterOwner(this);

	if(IsLocallyControlled())
	{
		const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		gameInstance->GetEventDispatcher()->OnEquippedWeapon.Broadcast(weapon);
	}
}

void ACharacterBase::UnEquipWeapon() const
{
	mEquipedWeapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	mEquipedWeapon->Destroy();

	if(IsLocallyControlled())
	{
		const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		gameInstance->GetEventDispatcher()->OnUnEquippedWeapon.Broadcast();
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

void ACharacterBase::UpdateHealth(const FTakeDamageData& damage)
{
	/*mGameplayAbilityAttributes->SetHealth(damage.health);
	auto currentHealth = mGameplayAbilityAttributes->GetHealth();
	
	//Client specific
	if(IsLocallyControlled())
	{
		//Update health hud
		const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		gameInstance->GetEventDispatcher()->OnRefreshHealth.Broadcast(currentHealth);
	}
	
	if(!IsLocallyControlled())
	{
		//Remote specific no server
		//only the remote where the playerCauser is autonomous (is the player who shoot without taking into accout the server case)
		//can see the hit points over the damaged.
		if(damage.playerCauser->GetLocalRole() == ENetRole::ROLE_AutonomousProxy)
		{
			//notify to the character recieved damage, to show hit points
			OnTakenDamage(damage.damage, damage.playerCauser->GetActorLocation(), currentHealth);
			if(!mGameplayAbilityAttributes->IsAlive())
			{
				//notify the character has dead
				BP_OnDead();
			}
		}
	}
	
	if(HasAuthority()) //Server specific
	{
		//And also if we are in the server, then if the causer is locallycontrolled
		if(damage.playerCauser->IsLocallyControlled())
		{
			OnTakenDamage(damage.damage, damage.playerCauser->GetActorLocation(), currentHealth);
			if(!mGameplayAbilityAttributes->IsAlive())
			{
				BP_OnDead();				
			}
		}
	}*/

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
	UnEquipWeapon();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECR_Ignore);
	
	if(IsLocallyControlled())
	{
		DisableInput(Cast<APlayerController>(GetController()));
	}

	DoDieClient();
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

void ACharacterBase::OnRep_TakeDamageData()
{
	UpdateHealth(mDamageCauser);
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

	//only remotes need to play animation
	PlayMontage(FireAnimation3P, GetMesh());
	const auto weapon = GetEquippedWeapon();
	if(weapon.GetObject() == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][ACharacterBase::FillWithWeaponMuzzleLocationAndRotation] weapon is null"), *GetName());
		return;
	}
	weapon->FireClient(false);
}

//TODO en este punto vemos que las armas se están haciendo spawn en
//cliente. Esto hace que no estén sincronizadas entre clientes pues
//no existen en el servidor. Por ahora es aceptable.
//No hay nada que le pueda pasar a un arma en local que se tenga
//que ver en los remotos. Pero, en el momento que este arma la
//pueda recojer otro jugador entonces sí va a tener que existir
//una sola instancia en servidor pues solo la podrá tener un jugador.
//Entonces, habrá que hacer el spawn en Server, hacer que el weapon
//esté replicada y notificar a los clientes autonomos para que
//vuelvan a inicializar.
void ACharacterBase::MulticastSpawnWeapon_Implementation()
{
	SpawnWeapon();
	Initialize(IsLocallyControlled());
}

void ACharacterBase::SpawnWeapon()
{
	if(mEquipedWeapon != nullptr)
	{
		return;
	}
	if(WeaponClass == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[ACharacterBase::SpawnWeapon] weapon class undefined"));
		return;
	}
	const auto weapon = GetWorld()->SpawnActor<AWeaponBase>(WeaponClass);
	if(weapon != nullptr && weapon->GetClass()->ImplementsInterface(UIWeapon::StaticClass()))
	{
		mEquipedWeapon = TScriptInterface<IIWeapon>(weapon);		
	}
	else
	{
		UE_LOG(LogCharacter, Error, TEXT("[ACharacterBase::SpawnWeapon] weapon not implementing IIWeapon"));
	}
}