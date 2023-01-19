// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGunComponent.h"
#include "Components/ActorComponent.h"
#include "CombatComponent.generated.h"

class ACharacterBase;
class IWeapon;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UCombatComponent : public UActorComponent, public IGunComponent
{
private:
	GENERATED_BODY()

	UPROPERTY(Replicated)
	TScriptInterface<IWeapon> mEquippedWeapon;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (AllowPrivateAccess = "true"))
	UAnimMontage* ShootingAnimation;
	
	UPROPERTY(Replicated)
	bool mIsAiming;
	
	float mAimWalkSpeed;
	
public:	
	// Sets default values for this component's properties
	UCombatComponent();

	virtual bool EquipWeapon(TScriptInterface<IWeapon> weapon, const FName& socketName) override;
	virtual bool UnEquipWeapon() const override;

	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual TScriptInterface<IWeapon> GetEquippedWeapon() const override;

	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool HasWeaponEquipped() const override { return GetEquippedWeapon() != nullptr; }

	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool CanShoot() const override;
	virtual void Shoot() const override;
	virtual UAnimMontage* GetShootingMontage() const override { return ShootingAnimation; }
	
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool IsAiming() const override{ return mIsAiming; }
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual void StartAiming() override;
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual void StopAiming() override;
	UFUNCTION(BlueprintCallable, Category = "IGunComponent")
	virtual bool CanAim() const override;
	
	void SetupLeftHandSocketTransform(const ACharacterBase* character) const;
private:
	//void OnRep_EquippedWeapon();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
};
