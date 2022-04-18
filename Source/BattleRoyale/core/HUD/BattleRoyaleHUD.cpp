// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleHUD.h"

//#include "AutomationBlueprintFunctionLibrary.h"
#include "AnnouncementsHUD.h"
#include "CharacterHUD.h"
#include "Kismet/GameplayStatics.h"
#include "Logging/LogMacros.h"


void ABattleRoyaleHUD::BeginPlay()
{
	Super::BeginPlay();

	if (CharacterHUDWidgetClasses.Num() <= 0)
	{
		//if(!UAutomationBlueprintFunctionLibrary::AreAutomatedTestsRunning()) doesn't work
		{
			UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleHUD::BeginPlay Character HUD has no widgets defined"));
		}

		return;
	}
	CreateHUD<ACharacterHUD>(mCharacterHUD, CharacterHUDWidgetClasses);
	CreateHUD<AAnnouncementsHUD>(mAnnouncementsHUD,AnnouncementsHUDWidgetClasses);
	CreateHUD<AMenuHUD>(mMenuHUD, MenuHUDWidgetClasses);
}
