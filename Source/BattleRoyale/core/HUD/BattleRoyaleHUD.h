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
	
	UPROPERTY()
	UUserWidget* mCharacterHUDWidget;

protected:
	virtual void BeginPlay() override;

private:
	void CreateCharacterHUD(TArray<TSubclassOf<UUserWidget>> widgetClasses);
	
	class ACharacterHUD* mCharacterHUD;
};

