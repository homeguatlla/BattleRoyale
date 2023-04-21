// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"

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
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, mEquippedWeapon);
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

		CalculateInterpolatedFOV(DeltaTime);
	}
	if(IsDebugEnabled)
	{
		DebugDrawAiming();
	}
}

void UCombatComponent::OnRep_EquippedWeapon() const
{
	//This rep notify is to inform the clients when the client just equipped a weapon,
	//and then show the crosshairs.
	if(!mCharacter || !mCharacter->IsLocallyControlled())
	{
		return;
	}
	GetGameInstance()->GetEventDispatcher()->OnEquippedWeapon.Broadcast(GetEquippedWeapon());
}

bool UCombatComponent::EquipWeapon(TScriptInterface<IWeapon> weapon, const FName& socketName)
{
	if(weapon == nullptr)
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::EquipWeapon] weapon is null"), *GetName());
		return false;
	}
	
	mEquippedWeapon = weapon;
	mEquippedWeapon->SetCharacterOwner(mCharacter);
	SetupLeftHandSocketTransform(mCharacter);
	
	return true;
}

bool UCombatComponent::UnEquipWeapon() const
{
	mEquippedWeapon->Destroy(); //TODO esto está mal creo debería dejarla tal cual y hacerle un dropped en el character

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
	mCharacter->GetAbilitySystemComponentBase()->RemoveGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
}

bool UCombatComponent::CanAim() const
{
	return HasWeaponEquipped() && !mIsAiming;
}

FVector UCombatComponent::GetShootingTargetLocation() const
{
	return CalculateShootingTargetData().targetLocation;
}

void UCombatComponent::Shoot() const
{
	if(!HasWeaponEquipped())
	{
		UE_LOG(LogCharacter, Error, TEXT("[%s][UCombatComponent::Shoot] There is no weapon equipped"), *GetName());
		return;
	}
	
	const auto shootingTargetData = CalculateShootingTargetData();
	const auto weapon = GetEquippedWeapon();
	weapon->Fire(shootingTargetData.targetLocation);
}

void UCombatComponent::SetupLeftHandSocketTransform(const ACharacterBase* character) const
{
	if(!HasWeaponEquipped())
		return;

	const auto leftHandTransform = GetEquippedWeapon()->SaveLeftHandSocketTransform();
	FVector newPosition;
	FRotator newRotator;
	character->GetMesh()->TransformToBoneSpace(
		FName("hand_r"),
		leftHandTransform.GetLocation(),
		FRotator::ZeroRotator,
		newPosition,
		newRotator);

	GetEquippedWeapon()->SetupLeftHandSocketTransform(newPosition, newRotator);
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
	//TODO el spread ahora mismo depende de la velocidad.
	//en el curso depende también de si estás aiming, se estrecha,
	//si disparas lo setea a un valor fijo y luego interpola a cero ese factor.
	//siempre está interpolando con DeltaTime.
	if(const auto characterInterface = Cast<IICharacter>(GetOwner()))
	{
		auto velocity = characterInterface->GetCurrentVelocity();
		//velocity.Z = 0.0f;
		const auto maxWalkSpeed = characterInterface->GetMaxWalkSpeed();

		//From velocity [0, maxWalkSpeed] to [0, 1] depending on the velocity
		return FMath::GetMappedRangeValueClamped(FVector2D(0, maxWalkSpeed), FVector2D(0.0f, 1.0f), velocity.Size());
	}
	return 0.0f;
}

static FVector normalCameraOffset;

void UCombatComponent::CalculateInterpolatedFOV(float DeltaTime)
{
	//If zoom is very large and we see things blured, there are two parameters in the camera we can modify in order
	//to make image sharper and without blur. DepthOfField/FocalDistance (10000)
	//and Camera/Aperture(F-stop) for instance with a value of 32
	//look at 87-Zoom While Aiming of the Multiplayer shooter course.
	check(mCharacter->GetCamera());

	const auto camera = mCharacter->GetCamera();
	const auto weapon = GetEquippedWeapon();
	const auto mesh = mCharacter->GetMesh();
	
	if(mIsAiming)
	{
		mCurrentFOV = FMath::FInterpTo(mCurrentFOV, weapon->GetZoomedFOV(), DeltaTime, weapon->GetZoomInterpolationSpeed());
		
		const auto weaponDirection = weapon->GetForwardVector();
		const auto cameraDirection = camera->GetForwardVector();
		auto cameraLocation = camera->GetComponentLocation();
		
		GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Cyan, FString::Printf(TEXT("Camera: %s"), *cameraDirection.ToString()));
		GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Cyan, FString::Printf(TEXT("Weapon: %s"), *weaponDirection.ToString()));
		
		DrawDebugLine(GetWorld(), weapon->GetMuzzleLocation(), weapon->GetMuzzleLocation() + weaponDirection * 1000, FColor::Blue);
		DrawDebugSphere(GetWorld(), weapon->GetMuzzleLocation(), 3, 10, FColor::Blue);
		
		//Esto es necesario para que coincidan exactamente los dos vectores de dirección de la cámara y el weapon
		const auto rotator = camera->GetComponentRotation();
		const auto result = UKismetMathLibrary::ComposeRotators(FRotator(0.0f, -90.0f, 0.0f), rotator);
		weapon->StartAiming(FVector::Zero(), result);

		camera->SetRelativeLocation(mDefaultCameraRelativeLocation + FVector(0.0f, 16.7, 0.0f));
		cameraLocation = camera->GetComponentLocation();
		DrawDebugLine(GetWorld(), cameraLocation , cameraLocation + cameraDirection * 1000, FColor::Green);
		DrawDebugSphere(GetWorld(), cameraLocation, 3, 10, FColor::Green);


		
		const auto weaponCrosshairTransform = weapon->GetCrosshairSocketTransform();
		DrawDebugLine(GetWorld(), weaponCrosshairTransform.GetLocation(), weaponCrosshairTransform.GetLocation() + weaponCrosshairTransform.GetRotation().GetUpVector() * 100, FColor::Cyan);
		const auto distanceCrosshairToCameraLine = FMath::PointDistToLine(weaponCrosshairTransform.GetLocation(), cameraDirection, cameraLocation);
		GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Cyan, FString::Printf(TEXT("Distance: %f"), distanceCrosshairToCameraLine));
		DrawDebugLine(
					GetWorld(),
					weaponCrosshairTransform.GetLocation(),
					weaponCrosshairTransform.GetLocation() + weaponCrosshairTransform.GetRotation().GetUpVector() * distanceCrosshairToCameraLine,
					FColor::Black,
					false,
					-1,
					0,
					0.3);
		
		
	/*
		mCurrentCameraRelativeLocation = FMath::Lerp(mCurrentCameraRelativeLocation,
													 normalCameraOffset + FVector(0.0f, 16.7f, distanceCrosshairToCameraLine),
													 DeltaTime * weapon->GetZoomInterpolationSpeed());*/
		mCurrentCameraRelativeLocation = mDefaultCameraRelativeLocation + FVector(0.0f, 16.7f, distanceCrosshairToCameraLine);
		camera->SetRelativeLocation(mCurrentCameraRelativeLocation);
	
		DrawDebugLine(GetWorld(), camera->GetComponentLocation(), camera->GetComponentLocation() + camera->GetForwardVector() * 500, FColor::Red, false, -1, 0, 0.3);

		
		const auto newDistanceLines = FMath::PointDistToLine(cameraLocation, cameraDirection, camera->GetComponentLocation());
		GEngine->AddOnScreenDebugMessage(-1, 0.1, FColor::Red, FString::Printf(TEXT("DiffDistance: %f"), distanceCrosshairToCameraLine - newDistanceLines));
	}
	else
	{
		mCurrentFOV = FMath::FInterpTo(mCurrentFOV, mDefaultFOV, DeltaTime, ZoomInterpolationFOV);
		mCurrentCameraRelativeLocation = FMath::Lerp<FVector>(
			mCurrentCameraRelativeLocation,
			mDefaultCameraRelativeLocation,
			DeltaTime * weapon->GetZoomInterpolationSpeed());
		camera->SetRelativeLocation(mCurrentCameraRelativeLocation);
		//weapon->StopAiming();
	}
	
	//camera->SetFieldOfView(mCurrentFOV);
	
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

void UCombatComponent::DebugDrawAiming() const
{
	if(!mCharacter->IsLocallyControlled() || !HasWeaponEquipped())
	{
		return;
	} 
	const auto muzzleLocation = GetEquippedWeapon()->GetMuzzleLocation();
	const auto shootingTargetData = CalculateShootingTargetData();
	DrawDebugSphere(GetWorld(), muzzleLocation, 5, 12, FColor::White, false);
	DrawDebugSphere(GetWorld(), muzzleLocation, 3, 12, FColor::Blue, false);
	DrawDebugLine(GetWorld(), muzzleLocation, shootingTargetData.targetLocation, FColor::Blue, false);
}
