// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BattleRoyaleHUD.generated.h"

UCLASS()
class BATTLEROYALE_API ABattleRoyaleHUD : public AHUD
{
	GENERATED_BODY()

public:
	
	/** Blueprint class which displays the ability */
	UPROPERTY(EditAnywhere, Category = "Character HUDs")
	TArray<TSubclassOf<UUserWidget>> CharacterHUDWidgetClasses;

	UPROPERTY(EditAnywhere, Category = "Announcements HUDs")
	TArray<TSubclassOf<UUserWidget>> AnnouncementsHUDWidgetClasses;
	
	UPROPERTY()
	UUserWidget* mCharacterHUDWidget;
	
	UPROPERTY()
	UUserWidget* mAnnouncementsHUDWidget;

protected:
	virtual void BeginPlay() override;

private:
	void CreateCharacterHUD(TArray<TSubclassOf<UUserWidget>> widgetClasses);
	void CreateAnnouncementsHUD(TArray<TSubclassOf<UUserWidget>> widgetClasses);
	
	class ACharacterHUD* mCharacterHUD;
	class AAnnouncementsHUD* mAnnouncementsHUD;
};

