// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"
#include "Blueprint/UserWidget.h"
#include "CharacterHUD.generated.h"

/**
 * Basic character HUD
 */

UCLASS()
class BATTLEROYALE_API ACharacterHUD : public AActor
{
	GENERATED_BODY()

public:
	ACharacterHUD();

	void Initialize(int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> CharacterHUDWidgetClasses);

private:
	void BindToDelegate();
	UFUNCTION()
	void OnEquippedWeapon(TScriptInterface<IWeapon> weapon);
	UFUNCTION()
	void OnUnEquippedWeapon();
	UFUNCTION()
	void OnRefreshCrosshair(float spread, AActor* targetActor, bool isAiming);
	UFUNCTION()
	void OnRefreshHealthReceived(float health);
	UFUNCTION()
	void OnRefreshAmmo(int32 ammo, int32 magazineCapacity);
	UFUNCTION()
	void OnPlayerDead();
	UFUNCTION()
	void OnRefreshNumKills(int numKills);
	UFUNCTION()
	void OnGameStarted();
	
public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
