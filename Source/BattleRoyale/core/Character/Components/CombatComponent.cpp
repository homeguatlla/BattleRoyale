// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/PickableObjects/Weapons/IWeapon.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "BattleRoyale/core/PickableObjects/Weapons/WeaponBase.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/KismetMathLibrary.h"

#define CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_STANDUP 16.7f
#define CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_CROUCH 5.8f

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	bWantsInitializeComponent = true;
}

void UCombatComponent::InitializeComponent()
{
	Super::InitializeComponent();

	mCharacter = Cast<ACharacterBase>(GetOwner());
	check(mCharacter);

	if(!mCharacter->HasAuthority())
	{
		return;
	}
	
	//Subscribe to pickup delegate
	if(const auto inventoryComponent = Cast<UInventoryComponent>(mCharacter->GetInventoryComponent().GetObject()))
	{
		inventoryComponent->OnEquippedPickableObjectDelegate.AddUObject(this, &ThisClass::OnEquippedPickableObject);
		inventoryComponent->OnDroppedPickableObject.AddUObject(this, &ThisClass::OnDroppedPickableObject);
	}
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, mIsAiming);
}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
	mAimWalkSpeed = mCharacter->GetMinWalkSpeed();
	SetCameraFOV(mCharacter->GetCamera()->FieldOfView);
	SetCameraRelativeLocation(mCharacter->GetCamera()->GetRelativeLocation());
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if(mCharacter && mCharacter->IsLocallyControlled() && HasWeaponEquipped())
	{
		const auto spread = CalculateCrosshairSpread();
		const auto shootingTargetData = CalculateShootingTargetData();
		//GEngine->AddOnScreenDebugMessage(-1, 0.05, FColor::Green, shootingTargetData.targetActor ? *shootingTargetData.targetActor->GetName() : *FString("No target"));
		//TODO igual podemos forzar aquí que el shootingTargetData.targetActor implemente una interfaz que afecte al crosshairs.
		//también lo puede validar el blueprint.
		GetGameInstance()->GetEventDispatcher()->OnRefreshCrosshair.Broadcast(spread, shootingTargetData.targetActor, mIsAiming);

		CalculateInterpolatedFOVAndCameraLocation(DeltaTime);
	}
	if(IsDebugEnabled)
	{
		DebugDrawAiming();
	}
}

bool UCombatComponent::EquipWeapon(TScriptInterface<IWeapon> weapon)
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	if(weapon == nullptr)
	{
		gameInstance->GetEventDispatcher()->OnRefreshAmmo.Broadcast(0,0);
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::EquipWeapon] weapon is null"), *GetName());
		return false;
	}
	
	mEquippedWeapon = weapon;
	SetupLeftHandSocketTransform(mCharacter);
	if(mCharacter->IsLocallyControlled())
	{
		
		gameInstance->GetEventDispatcher()->OnEquippedWeapon.Broadcast(GetEquippedWeapon());
		gameInstance->GetEventDispatcher()->OnRefreshAmmo.Broadcast(weapon->GetAmmo(), weapon->GetMagazineCapacity());
	}
	
	return true;
}

bool UCombatComponent::UnEquipWeapon()
{
	//mEquippedWeapon->SetCharacterOwner(nullptr);
	//Reset the equipped weapon
	mEquippedWeapon = TScriptInterface<IWeapon>();
	GetGameInstance()->GetEventDispatcher()->OnRefreshAmmo.Broadcast(0, 0);

	return true;
}

TScriptInterface<IWeapon> UCombatComponent::GetEquippedWeapon() const
{
	return mEquippedWeapon;
}

bool UCombatComponent::CanShoot() const
{
	return mEquippedWeapon && mEquippedWeapon->CanBeFired();
}

void UCombatComponent::StartAiming()
{
	mIsAiming = true;
	
	mCharacter->GetCharacterMovement()->MaxWalkSpeed = mAimWalkSpeed;
	
	mCharacter->GetAbilitySystemComponentBase()->AddGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
	mCharacter->GetAbilitySystemComponentBase()->CancelAbilitiesWithTags(FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT)));
}

void UCombatComponent::StopAiming()
{
	mIsAiming = false;
	mCharacter->GetCharacterMovement()->MaxWalkSpeed = mCharacter->GetMaxWalkSpeed();
	//Cuando cerramos el juego el player state es nulo y por tanto no hay GAS y peta aquí
	//si cerremos el juego mientras estás haciendo aiming
	if(const auto abilitySystemComponent = mCharacter->GetAbilitySystemComponentBase())
	{
		abilitySystemComponent->RemoveGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
	}
}

bool UCombatComponent::CanAim() const
{
	return HasWeaponEquipped() && !mIsAiming;
}

FVector UCombatComponent::GetShootingTargetLocation() const
{
	return CalculateShootingTargetData().targetLocation;
}

bool UCombatComponent::CanReload(const TScriptInterface<IIInventoryComponent> inventoryComponent) const
{
	if(!HasWeaponEquipped())
	{
		return false;
	}
	
	const auto weapon = GetEquippedWeapon();
	if(GetEquippedWeapon()->IsMagazineFull())
	{
		return false;
	}
	
	const auto ammoTypeNeeded = GetEquippedWeapon()->GetAmmoType();

	return inventoryComponent->HasAmmoOfType(ammoTypeNeeded);
}

void UCombatComponent::Reload(const TScriptInterface<IIInventoryComponent> inventoryComponent)
{
	//TODO quizá hay que volver a comprobar que pueda hacer el reload y volver a comprobar todo?
	const auto weapon = GetEquippedWeapon();

	check(weapon);
	
	const auto ammoTypeNeeded = weapon->GetAmmoType();
	const auto ammoNeeded = weapon->GetMagazineCapacity() - weapon->GetAmmo();

	const auto ammoFound = inventoryComponent->RemoveEnoughAmmo(ammoTypeNeeded, ammoNeeded);

	//If we can reload at least will be 1 ammo
	check(ammoFound > 0);

	weapon->Reload(ammoFound);

	//Update ammo HUD
	GetGameInstance()->GetEventDispatcher()->OnRefreshAmmo.Broadcast(weapon->GetAmmo(), weapon->GetMagazineCapacity());
}

void UCombatComponent::Shoot()
{
	//We shoot, and then if is still fire button pressed we start the timer.
	ShootOnce();
	
	const auto weapon = mEquippedWeapon;
	const auto playerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());

	const auto isFireInputPressed = IsInputPressedByActionName("Fire", playerController);
	if(weapon->IsAutomaticFireEnabled() && isFireInputPressed)
	{
		StartAutomaticFireTimer();
	}
}

void UCombatComponent::ShootOnce() const
{
	if(!HasWeaponEquipped())
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::Shoot] There is no weapon equipped"), *GetName());
		return;
	}
	
	const auto shootingTargetData = CalculateShootingTargetData();
	const auto weapon = mEquippedWeapon;

	weapon->Fire(shootingTargetData.targetLocation);
	GetGameInstance()->GetEventDispatcher()->OnRefreshAmmo.Broadcast(weapon->GetAmmo(), weapon->GetMagazineCapacity());
}

void UCombatComponent::OnEquippedPickableObject(TScriptInterface<IPickupObject> pickableObject)
{
	if(pickableObject.GetObject()->Implements<UWeapon>())
	{
		const TScriptInterface<IWeapon> weapon = pickableObject.GetObject();
		EquipWeapon(weapon);
		ClientEquipWeapon(weapon.GetObject());
	}
}

void UCombatComponent::OnDroppedPickableObject()
{
	if(UnEquipWeapon())
	{		
		if(mCharacter->IsLocallyControlled())
		{
			const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
			gameInstance->GetEventDispatcher()->OnUnEquippedWeapon.Broadcast();
		}
	}
}

void UCombatComponent::ClientEquipWeapon_Implementation(UObject* weapon)
{
	EquipWeapon(weapon);
}

void UCombatComponent::ReleaseTrigger()
{
	GetWorld()->GetTimerManager().ClearTimer(mAutomaticFireTimer);
}

void UCombatComponent::StartAutomaticFireTimer()
{
	const auto weapon = mEquippedWeapon;
	if(!weapon->IsAutomaticFireEnabled())
	{
		return;
	}
	
	GetWorld()->GetTimerManager().SetTimer(
		mAutomaticFireTimer,
		this,
		&ThisClass::OnAutomaticFire,
		weapon->GetAutomaticFireDelay(),
		true
		);
}

void UCombatComponent::OnAutomaticFire()
{
	//TODO lo que estaría bien es  que de alguna manera te pudieras suscribir a un evento que te diga cuando se ha dejado el trigger y entonces ejecutar
	//el ReleaseTrigger del combat component más que mirar si el fire está todavía pulsado
	//Claro el player controller podría estar suscrito al evento de Fire e informar al combatcomponent, pero no sé si esto choca con la abilidad.
	const auto playerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	const auto isFireInputPressed = IsInputPressedByActionName("Fire", playerController);
	const auto weapon = GetEquippedWeapon();
	
	if(!isFireInputPressed || !weapon->HasAmmo())
	{
		ReleaseTrigger();
		return;
	}
	ShootOnce();
}

void UCombatComponent::SetupLeftHandSocketTransform(const ACharacterBase* character) const
{
	if(!HasWeaponEquipped())
		return;

	const auto leftHandTransform = mEquippedWeapon->SaveLeftHandSocketTransform();
	FVector newPosition;
	FRotator newRotator;
	character->GetMesh()->TransformToBoneSpace(
		FName("hand_r"),
		leftHandTransform.GetLocation(),
		FRotator::ZeroRotator,
		newPosition,
		newRotator);

	mEquippedWeapon->SetupLeftHandSocketTransform(newPosition, newRotator);
}

FShootingData UCombatComponent::CalculateShootingTargetData() const
{
	const auto playerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());
	const auto hitResult = utils::UtilsLibrary::TraceLineSingleByChannelToCrosshair(GetWorld(), playerController, MaxShootingDistance);

	if(!hitResult.bBlockingHit)
	{
		return {hitResult.TraceEnd, nullptr};
	}
	
	return {hitResult.ImpactPoint, hitResult.GetActor()};
}

float UCombatComponent::CalculateCrosshairSpread() const
{
	if(const auto characterInterface = Cast<IICharacter>(GetOwner()))
	{
		const auto velocity = characterInterface->GetCurrentVelocity();
		const auto maxWalkSpeed = characterInterface->GetMaxWalkSpeed();

		//From velocity [0, maxWalkSpeed] to [0, 1] depending on the velocity
		return FMath::GetMappedRangeValueClamped(
			FVector2D(0, maxWalkSpeed),
			FVector2D(0.0f, 1.0f),
			velocity.Size());
	}
	return 0.0f;
}

static FVector normalCameraOffset;

void UCombatComponent::CalculateInterpolatedFOVAndCameraLocation(float DeltaTime)
{
	//If zoom is very large and we see things blured, there are two parameters in the camera we can modify in order
	//to make image sharper and without blur. DepthOfField/FocalDistance (10000)
	//and Camera/Aperture(F-stop) for instance with a value of 32
	//look at 87-Zoom While Aiming of the Multiplayer shooter course.
	check(mCharacter->GetCamera());

	const auto camera = mCharacter->GetCamera();
	const auto weapon = mEquippedWeapon;
	
	if(mIsAiming)
	{
		float relativeYOffsetToAlignCameraAndWeapon = 0.0f;
		float upCameraVectorSign = -1.0f;
		if(mCharacter->IsCrouching())
		{
			relativeYOffsetToAlignCameraAndWeapon = CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_CROUCH;
			upCameraVectorSign = -1.0f;
		}
		else
		{
			relativeYOffsetToAlignCameraAndWeapon = CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_STANDUP;
			upCameraVectorSign = 1.0f;
		}
		
		mCurrentFOV = FMath::FInterpTo(mCurrentFOV, weapon->GetZoomedFOV(), DeltaTime, weapon->GetZoomInterpolationSpeed());
	
		const auto cameraDirection = camera->GetForwardVector();
		
		//Rotate weapon to make camera direction and weapon direction both the same direction
		const auto rotator = camera->GetComponentRotation();
		const auto result = UKismetMathLibrary::ComposeRotators(FRotator(0.0f, -90.0f, 0.0f), rotator);
		weapon->StartAiming(FVector::Zero(), result);

		//Camera and Weapon directions. Must be parallel (the same vector)
		//GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Cyan, FString::Printf(TEXT("Camera: %s"), *cameraDirection.ToString()));
		//GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Cyan, FString::Printf(TEXT("Weapon: %s"), *weaponDirection.ToString()));

		//Weapon Line
		//DrawDebugLine(GetWorld(), weapon->GetMuzzleLocation(), weapon->GetMuzzleLocation() + weaponDirection * 1000, FColor::Blue);

		//Move camera to center on the weapon.
		camera->SetRelativeLocation(mDefaultCameraRelativeLocation + FVector(0.0f, relativeYOffsetToAlignCameraAndWeapon, 0.0f));
		const auto cameraLocation = camera->GetComponentLocation();
		
		//Camera line
		//DrawDebugLine(GetWorld(), cameraLocation , cameraLocation + cameraDirection * 1000, FColor::Green);

		const auto weaponCrosshairTransform = weapon->GetCrosshairSocketTransform();
		
		//Crosshair UP line
		//DrawDebugLine(GetWorld(), weaponCrosshairTransform.GetLocation(), weaponCrosshairTransform.GetLocation() + weaponCrosshairTransform.GetRotation().GetUpVector() * 100, FColor::Cyan);

		//Calculate the distance from the crosshair to the camera line (once aligned with the weapon)
		const auto distanceCrosshairToCameraLine = FMath::PointDistToLine(weaponCrosshairTransform.GetLocation(), cameraDirection, cameraLocation);
		//We have to move camera relative position in z, up to distanceCrosshairToCameraLine. But, camera can be rotated because of pitch.
		//So we cannot add relative z = distanceCrosshairToCameraLine because camera up vector can not be 1.
		//To calculate the distance to move we know that upvector.z * newDistance = distanceCrosshairToCameraLine, so newDistance = distanceCrosshairToCameraLine/cameraUp.z
		const auto cameraUpVector = camera->GetComponentTransform().GetRotation().GetUpVector();
		const auto relativeUpZ = distanceCrosshairToCameraLine / cameraUpVector.Z;
		mCurrentCameraRelativeLocation = FMath::Lerp(mCurrentCameraRelativeLocation,
															 mDefaultCameraRelativeLocation + FVector(0.0f, relativeYOffsetToAlignCameraAndWeapon, upCameraVectorSign * relativeUpZ),
															 DeltaTime * weapon->GetZoomInterpolationSpeed());
		
	
		
		//Decomment to see the difference between the final camera position and the current camera position
		//DrawDebugLine(GetWorld(), camera->GetComponentLocation(), camera->GetComponentLocation() + camera->GetForwardVector() * 500, FColor::Red, false, -1, 0, 0.3);
		//const auto newDistanceLines = FMath::PointDistToLine(cameraLocation, cameraDirection, camera->GetComponentLocation());
		//float diff = distanceCrosshairToCameraLine - newDistanceLines;
		//GEngine->AddOnScreenDebugMessage(-1, 0.1, fabs(diff) < 0.05 ? FColor::Green : FColor::Red, FString::Printf(TEXT("DiffDistance: %f"), diff));
	}
	else
	{
		mCurrentFOV = FMath::FInterpTo(mCurrentFOV, mDefaultFOV, DeltaTime, ZoomInterpolationFOV);
		mCurrentCameraRelativeLocation = FMath::Lerp<FVector>(
			mCurrentCameraRelativeLocation,
			mDefaultCameraRelativeLocation,
			DeltaTime * weapon->GetZoomInterpolationSpeed());
		weapon->StopAiming();
	}
	
	camera->SetFieldOfView(mCurrentFOV);
	camera->SetRelativeLocation(mCurrentCameraRelativeLocation);
}

UBattleRoyaleGameInstance* UCombatComponent::GetGameInstance() const
{
	return Cast<UBattleRoyaleGameInstance>(mCharacter->GetGameInstance());
}

void UCombatComponent::SetCameraFOV(float fov)
{
	mDefaultFOV = fov;
	mCurrentFOV = fov;
}

void UCombatComponent::SetCameraRelativeLocation(const FVector& location)
{
	mDefaultCameraRelativeLocation = location;
	mCurrentCameraRelativeLocation = mDefaultCameraRelativeLocation;
	normalCameraOffset = mDefaultCameraRelativeLocation;
}

bool UCombatComponent::IsInputPressedByActionName(const FName& ActionName, const APlayerController* PlayerController) const
{
	TArray <FInputActionKeyMapping> OutMappings;
	const UInputSettings* InputSettings = UInputSettings::GetInputSettings();
	InputSettings->GetActionMappingByName(ActionName, OutMappings);
		
	bool isPressed = false;
	for (auto&& inputActionKey : OutMappings)
	{
		if(PlayerController->IsInputKeyDown(inputActionKey.Key))
		{
			isPressed = true;
		}
	}
	return isPressed;
}


void UCombatComponent::DebugDrawAiming() const
{
	if(!mCharacter->IsLocallyControlled() || !HasWeaponEquipped())
	{
		return;
	} 
	const auto muzzleLocation = mEquippedWeapon->GetMuzzleLocation();
	const auto shootingTargetData = CalculateShootingTargetData();
	DrawDebugSphere(GetWorld(), muzzleLocation, 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	DrawDebugLine(GetWorld(), muzzleLocation, shootingTargetData.targetLocation, FColor::Blue, false);
}
