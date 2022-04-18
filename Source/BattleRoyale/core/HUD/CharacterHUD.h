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
	void OnEquippedWeapon(TScriptInterface<IIWeapon> weapon);
	UFUNCTION()
	void OnUnEquippedWeapon();
	UFUNCTION()
	void OnRefreshHealthReceived(float health);
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
