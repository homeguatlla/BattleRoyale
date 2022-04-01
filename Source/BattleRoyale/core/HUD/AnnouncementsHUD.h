// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AnnouncementsHUD.generated.h"

/**
 * Announcements HUD
 * To show messages not related to the character but to the game.
 */

UCLASS()
class BATTLEROYALE_API AAnnouncementsHUD : public AActor
{
	GENERATED_BODY()

public:
	AAnnouncementsHUD();

	void Initialize(int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> announcementsHUDWidgetClasses);

private:
	void BindToDelegate();
	
	UFUNCTION()
	void OnAnnounceNewDeath();
	
public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
