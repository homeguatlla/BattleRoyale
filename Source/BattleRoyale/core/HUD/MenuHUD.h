// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Utils/Stats/PlayerStatsData.h"
#include "Blueprint/UserWidget.h"
#include "MenuHUD.generated.h"

/**
 * Basic Menu HUD
 */

UCLASS()
class BATTLEROYALE_API AMenuHUD : public AActor
{
	GENERATED_BODY()

public:
	AMenuHUD();

	void Initialize(int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> MenuHUDWidgetClasses);

private:
	void BindToDelegate();

	UFUNCTION()
	void OnGameStarted();
	UFUNCTION()
	void OnGameOver();
	UFUNCTION()
	void OnRefreshCountDown(uint8 counter);
	UFUNCTION()
	void OnFinishCountDown();
	UFUNCTION()
	void OnSetVisibilityCountDown(bool visible);
	UFUNCTION()
	void OnShowStatsScreen(const FPlayerStatsData& playerStatsData);
	UFUNCTION()
	void OnHideStatsScreen();
	
public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
