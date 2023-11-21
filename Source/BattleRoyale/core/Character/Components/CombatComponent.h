// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGunComponent.h"
#include "PickupSelectorComponent.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class IPickupObject;
class UBattleRoyaleGameInstance;
class ACharacterBase;
class IWeapon;

USTRUCT()
struct BATTLEROYALE_API FShootingData
{
	GENERATED_BODY()
	
	FVector targetLocation;
	UPROPERTY()
	AActor* targetActor;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UCombatComponent : public UActorComponent, public IGunComponent
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatComponent", meta = (AllowPrivateAccess = "true"))
	float MaxShootingDistance = 100000.0f;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatComponent", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShootingAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "CombatComponent", meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ReloadAnimation;
    	
	UPROPERTY(EditDefaultsOnly, Category = "CombatComponent")
	bool IsDebugEnabled { false };
	
	UPROPERTY(EditAnywhere, Category = "CombatComponent")
	float ZoomedFOV = 30.0f;
	
	UPROPERTY(EditAnywhere, Category = "CombatComponent")
	float ZoomInterpolationFOV = 20.0f;
	

public:	
	// Sets default values for this component's properties
	UCombatComponent();

	virtual void InitializeComponent() override;
	
	virtual bool UnEquipWeapon() override;

	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual TScriptInterface<IWeapon> GetEquippedWeapon() const override;

	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool HasWeaponEquipped() const override { return GetEquippedWeapon() != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool CanShoot() const override;
	virtual void Shoot() override;
	virtual void ReleaseTrigger() override;
	virtual UAnimMontage* GetShootingMontage() const override { return ShootingAnimation; }
	virtual UAnimMontage* GetReloadingMontage() const override { return ReloadAnimation; }
	
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool IsAiming() const override{ return mIsAiming; }
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual void StartAiming() override;
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual void StopAiming() override;
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool CanAim() const override;
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual FVector GetShootingTargetLocation() const override;

	virtual bool CanReload(const TScriptInterface<IIInventoryComponent> inventoryComponent) const override;
	virtual void Reload(TScriptInterface<IIInventoryComponent> inventoryComponent) override;
	virtual void SetupLeftHandSocketTransform(const ACharacterBase* character) const override;

protected:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	bool IsInputPressedByActionName(const FName& ActionName, const APlayerController* PlayerController) const;
	
	FShootingData CalculateShootingTargetData() const;
	float CalculateCrosshairSpread() const;
	void CalculateInterpolatedFOVAndCameraLocation(float DeltaTime);

	UBattleRoyaleGameInstance* GetGameInstance() const;
	void SetCameraFOV(float fov);
	void SetCameraRelativeLocation(const FVector& location);

	void StartAutomaticFireTimer();
	void OnAutomaticFire() ;
	void ShootOnce() const;

	bool EquipWeapon(TScriptInterface<IWeapon> weapon);
	void OnEquippedPickableObject(TScriptInterface<IPickupObject> pickableObject);
	void OnDroppedPickableObject();

	UFUNCTION(Server, Reliable)
	void ServerReload(UInventoryComponent* inventoryComponent);
	
	void DebugDrawAiming() const;
	
	UPROPERTY()
	TScriptInterface<IWeapon> mEquippedWeapon = nullptr;
	
	bool mIsAutomaticFireOn = false;
	FTimerHandle mAutomaticFireTimer;
	
	UPROPERTY()
	ACharacterBase* mCharacter;
	
	/**
	 * Aiming and FOV
	 */
	UPROPERTY(Replicated)
	bool mIsAiming = false;
	
	float mDefaultFOV;
	float mCurrentFOV;
	
	FVector mDefaultCameraRelativeLocation;
	FVector mCurrentCameraRelativeLocation;

	float mAimWalkSpeed;
};
