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
	
	UFUNCTION()
	void OnUpdateHealthReceived(float health);
	
	//inherit from IHealthHUD
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Health")
	void OnUpdateHealth(float health);

private:
	void BindToDelegate();
	UFUNCTION()
	void OnEquippedWeapon(TScriptInterface<IIWeapon> weapon);

public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
