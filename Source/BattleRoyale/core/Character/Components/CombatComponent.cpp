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

const float UCombatComponent::CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_STANDUP = 16.7f;
const float UCombatComponent::CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_CROUCH = 5.8f;

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

	//Subscribe to pickup delegate
	if(const auto inventoryComponent = Cast<UInventoryComponent>(mCharacter->GetInventoryComponent().GetObject()))
	{
		inventoryComponent->OnEquippedWeaponDelegate.AddUObject(this, &ThisClass::OnEquippedWeapon);
		inventoryComponent->OnPickedUpOrDroppedAmmoDelegate.AddUObject(this, &ThisClass::OnPickedOrDroppedUpAmmo);
		inventoryComponent->OnDroppedPickableObjectDelegate.AddUObject(this, &ThisClass::OnDroppedPickableObject);
		inventoryComponent->OnDroppedItemWeaponDelegate.AddUObject(this, &ThisClass::OnDroppedWeapon);
		inventoryComponent->OnUnEquippedWeaponDelegate.AddUObject(this, &ThisClass::OnUnEquipWeapon);
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

	if(GIsAutomationTesting)
	{
		return;
	}
	
	if(mCharacter && mCharacter->IsLocallyControlled() && HasWeaponEquipped())
	{
		const auto spread = CalculateCrosshairSpread();
		const auto shootingTargetData = CalculateShootingTargetData();
		//GEngine->AddOnScreenDebugMessage(-1, 0.05, FColor::Green, shootingTargetData.targetActor ? *shootingTargetData.targetActor->GetName() : *FString("No target"));
		//TODO igual podemos forzar aquí que el shootingTargetData.targetActor implemente una interfaz que afecte al crosshairs.
		//también lo puede validar el blueprint.
		const auto isMagazineAlmostEmpty = IsMagazineAlmostEmpty();
		GetGameInstance()->GetEventDispatcher()->OnRefreshCrosshair.Broadcast(spread, shootingTargetData.targetActor, mIsAiming, isMagazineAlmostEmpty);

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
		if(mCharacter->IsLocallyControlled())
		{
			gameInstance->GetEventDispatcher()->OnRefreshAmmo.Broadcast(0,0);
		}
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

bool UCombatComponent::IsReloading() const
{
	return mCharacter->GetAbilitySystemComponentBase()->HasGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_RELOADING));
}

void UCombatComponent::StartAiming()
{
	mIsAiming = true;
	
	mCharacter->GetAbilitySystemComponentBase()->AddGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
}

void UCombatComponent::StopAiming()
{
	mIsAiming = false;
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
	if(!GetOwner()->HasAuthority())
	{
		return;
	}
	ServerReload(Cast<UInventoryComponent>(inventoryComponent.GetObject()));
}

bool UCombatComponent::CanSwapWeapons(const TScriptInterface<IIInventoryComponent> inventoryComponent) const
{
	return inventoryComponent->GetTotalWeapons() > 0;
}

void UCombatComponent::SwapWeaponsServer(const TScriptInterface<IIInventoryComponent> inventoryComponent)
{
	if(!GetOwner()->HasAuthority())
	{
		return;
	}
	const auto currentItem = inventoryComponent->GetEquippedItem();
	const auto nextWeaponItemInstance = inventoryComponent->GetNextWeaponDifferentThan(currentItem);
	if(!nextWeaponItemInstance)
	{
		return;
	}
	inventoryComponent->UnEquipItem();
	if(!inventoryComponent->EquipItem(nextWeaponItemInstance))
	{
		//TODO hay que ver que hacemos aquí
		check(false);
	}
}

void UCombatComponent::Shoot()
{
	//We shoot, and then if is still fire button pressed we start the timer.
	ShootOnce();

	if(!CanShoot())
	{
		return;
	}
	
	const auto weapon = mEquippedWeapon;
	const auto playerController = Cast<APlayerController>(GetOwner()->GetInstigatorController());

	const auto isFireInputPressed = IsInputPressedByActionName("Fire", playerController);
	if(weapon->IsAutomaticFireEnabled() && isFireInputPressed && !mIsAutomaticFireOn)
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

	//DrawDebugSphere(GetWorld(), shootingTargetData.targetLocation, 10, 10, FColor::Yellow, false, 5, 0, 1);
	weapon->Fire(shootingTargetData.targetLocation);
}

void UCombatComponent::OnEquippedWeapon(TScriptInterface<IWeapon> weapon, int32 totalAmmo)
{
	EquipWeapon(weapon);
	if(mCharacter->IsLocallyControlled())
	{
		GetGameInstance()->GetEventDispatcher()->OnRefreshTotalAmmo.Broadcast(totalAmmo);
	}
}

void UCombatComponent::OnPickedOrDroppedUpAmmo(EAmmoType type, int32 totalAmmo)
{
	if(!mCharacter->IsLocallyControlled())
	{
		return;
	}
	if(!HasWeaponEquipped())
	{
		return;
	}
	if(GetEquippedWeapon()->GetAmmoType() != type)
	{
		return;
	}
	
	GetGameInstance()->GetEventDispatcher()->OnRefreshTotalAmmo.Broadcast(totalAmmo);
}

void UCombatComponent::OnDroppedPickableObject()
{
}

void UCombatComponent::OnDroppedWeapon(bool isEquipped)
{
	if(isEquipped)
	{
		UnEquipWeapon();
		if(mCharacter->IsLocallyControlled())
		{
			const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
			const auto eventDispatcher = gameInstance->GetEventDispatcher();
			eventDispatcher->OnUnEquippedWeapon.Broadcast();
			eventDispatcher->OnRefreshAmmo.Broadcast(0, 0);
			eventDispatcher->OnRefreshTotalAmmo.Broadcast(0);
		}
	}
}

void UCombatComponent::OnUnEquipWeapon(TScriptInterface<IWeapon> weapon)
{
	OnDroppedWeapon(HasWeaponEquipped() && GetEquippedWeapon() == weapon);
}

void UCombatComponent::ServerReload_Implementation(UInventoryComponent* inventoryComponent)
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

	MulticastNotifyTotalAmmo(inventoryComponent->GetTotalAmmoOfType(ammoTypeNeeded));
}

void UCombatComponent::MulticastNotifyTotalAmmo_Implementation(int GetTotalAmmoOfType)
{
	if(!mCharacter->IsLocallyControlled())
	{
		return;
	}
	GetGameInstance()->GetEventDispatcher()->OnRefreshTotalAmmo.Broadcast(GetTotalAmmoOfType);
}

void UCombatComponent::ReleaseTrigger()
{
	GetWorld()->GetTimerManager().ClearTimer(mAutomaticFireTimer);
	mIsAutomaticFireOn = false;
}

void UCombatComponent::StartAutomaticFireTimer()
{
	const auto weapon = mEquippedWeapon;
	if(!weapon->IsAutomaticFireEnabled())
	{
		return;
	}
	
	mIsAutomaticFireOn = true;
	
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
		if(mCharacter->IsCrouching())
		{
			relativeYOffsetToAlignCameraAndWeapon = CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_CROUCH;
		}
		else
		{
			relativeYOffsetToAlignCameraAndWeapon = CAMERA_RELATIVE_Y_OFFSET_TO_ALIGN_CAMERA_WITH_WEAPON_STANDUP;
		}
		
		mCurrentFOV = FMath::FInterpTo(mCurrentFOV, weapon->GetZoomedFOV(), DeltaTime, weapon->GetZoomInterpolationSpeed());
	
		//Read the Controller's rotation directly instead of camera->GetComponentRotation(): with
		//bUsePawnControlRotation, the camera component only copies the pawn's control rotation inside
		//GetCameraView(), called by the PlayerCameraManager - not guaranteed to happen before this tick reads it.
		//That meant we could be reading last frame's camera rotation, a constant one-frame lag that was
		//invisible at normal speed but showed up as the weapon stepping to catch up to the crosshair in slomo.
		const auto rotator = mCharacter->GetControlRotation();
		const auto cameraDirection = rotator.Vector();

		//Rotate weapon to make camera direction and weapon direction both the same direction
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

		//GetCrosshairSocketTransform() reads the mesh's CURRENT socket transform, which depends on whichever
		//hand_r rotation the arms AnimBP last evaluated - that may not be this frame's yet (animation and this
		//component tick independently), and any such staleness gets amplified by the lever arm out to the crosshair,
		//showing up as vertical vibration/lag while actively looking around. To avoid that race, we compute the
		//crosshair's expected world location ourselves: read hand_r's position (translation changes slowly, from
		//locomotion, so staleness there is imperceptible) but use OUR OWN freshly computed rotation (the same one
		//the AnimBP applies to hand_r this frame - see FirstPersonAnimationInstance::CheckEquippedToMakeWeaponAimsToCrosshair)
		//instead of reading a rotation back. The crosshair's offset from hand_r is fixed (the weapon is rigidly
		//attached to it), so converting its current world location into hand_r's bone space gives that constant offset.
		const auto handRWorldPosition = mCharacter->GetMesh()->GetSocketLocation(FName("hand_r"));
		FVector crosshairLocalOffsetFromHandR;
		FRotator unusedRotator;
		mCharacter->GetMesh()->TransformToBoneSpace(
			FName("hand_r"),
			weapon->GetCrosshairSocketTransform().GetLocation(),
			FRotator::ZeroRotator,
			crosshairLocalOffsetFromHandR,
			unusedRotator);
		const auto expectedHandRRotation = UKismetMathLibrary::MakeRotFromY(-cameraDirection);
		const auto expectedCrosshairWorldLocation = handRWorldPosition + expectedHandRRotation.RotateVector(crosshairLocalOffsetFromHandR);

		//Signed perpendicular distance from the crosshair to the camera line (once aligned with the weapon).
		//FMath::PointDistToLine returns an unsigned magnitude, which forced upCameraVectorSign below to guess
		//which side to correct towards; near the line that guess is often wrong, which showed up as the weapon
		//randomly landing above or below screen center. Projecting onto the camera's own up vector (not world Z,
		//which is only correct at zero pitch) gives the real sign while keeping the same magnitude/scale PointDistToLine used.
		const auto cameraUpVector = camera->GetComponentTransform().GetRotation().GetUpVector();
		const auto closestPointOnCameraLine = FMath::ClosestPointOnInfiniteLine(cameraLocation, cameraLocation + cameraDirection, expectedCrosshairWorldLocation);
		const auto perpendicularOffsetToCameraLine = expectedCrosshairWorldLocation - closestPointOnCameraLine;
		const auto signedDistanceCrosshairToCameraLine = FVector::DotProduct(perpendicularOffsetToCameraLine, cameraUpVector);
		//We have to move camera relative position in z, up to signedDistanceCrosshairToCameraLine. But, camera can be rotated because of pitch.
		//So we cannot add relative z = signedDistanceCrosshairToCameraLine because camera up vector can not be 1.
		//To calculate the distance to move we know that upvector.z * newDistance = signedDistanceCrosshairToCameraLine, so newDistance = signedDistanceCrosshairToCameraLine/cameraUp.z
		const auto relativeUpZ = signedDistanceCrosshairToCameraLine / cameraUpVector.Z;
		//relativeUpZ changes every frame with pitch, so it can't go through the same Lerp as the Y offset below:
		//lerping toward a continuously moving target never catches up while you're still moving, only once you stop
		//(that showed up as vertical lag while looking up/down). Only the Y offset (crouch/standup, a discrete
		//state) benefits from being smoothed; the vertical crosshair alignment is applied directly instead.
		mCurrentCameraRelativeLocation = FMath::Lerp(mCurrentCameraRelativeLocation,
															 mDefaultCameraRelativeLocation + FVector(0.0f, relativeYOffsetToAlignCameraAndWeapon, 0.0f),
															 DeltaTime * weapon->GetZoomInterpolationSpeed());
		mCurrentCameraRelativeLocation.Z = mDefaultCameraRelativeLocation.Z + relativeUpZ;
		
	
		
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

bool UCombatComponent::IsMagazineAlmostEmpty() const
{
	if(!HasWeaponEquipped())
	{
		return false;
	}
	
	const auto weapon = GetEquippedWeapon();
	const auto ammo = weapon->GetAmmo();
	const auto magazine = weapon->GetMagazineCapacity();

	return ammo <= magazine * MagazineAlmostEmptyPercentage;
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
