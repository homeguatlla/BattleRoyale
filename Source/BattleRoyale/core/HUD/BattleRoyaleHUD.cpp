// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleHUD.h"

//#include "AutomationBlueprintFunctionLibrary.h"
#include "AnnouncementsHUD.h"
#include "CharacterHUD.h"
#include "Logging/LogMacros.h"
#include "Visor/VisorPrototype.h"


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
	mCharacterHUD = CreateHUD<ACharacterHUD>(CharacterHUDWidgetClasses);
	mAnnouncementsHUD = CreateHUD<AAnnouncementsHUD>(AnnouncementsHUDWidgetClasses);
	mMenuHUD = CreateHUD<AMenuHUD>(MenuHUDWidgetClasses);
	mInventoryHUD = CreateHUD<AInventoryHUD>(MaxInventoryItems, InventoryHUDWidgetClasses);
	mNetworkHUD = CreateHUD<ANetworkHUD>(NetworkHUDWidgetClasses);
}

UVisorPrototype* ABattleRoyaleHUD::GetVisorPrototype()
{
	if (!mVisorPrototype)
	{
		mVisorPrototype = NewObject<UVisorPrototype>(this);
	}
	return mVisorPrototype;
}

void ABattleRoyaleHUD::VisorPrototype(int32 mode)
{
	GetVisorPrototype()->SetMode(GetOwningPlayerController(), mode);
}

void ABattleRoyaleHUD::VisorTune(float arcAngle, float distance)
{
	GetVisorPrototype()->Tune(arcAngle, distance);
}

void ABattleRoyaleHUD::VisorWarp(float curvature, float scanlines, float aberration)
{
	GetVisorPrototype()->SetWarp(curvature, scanlines, aberration);
}

void ABattleRoyaleHUD::VisorHealth(float health)
{
	GetVisorPrototype()->SetHealth(health);
}

void ABattleRoyaleHUD::VisorGlow(float strength, float radius)
{
	GetVisorPrototype()->SetGlow(strength, radius);
}
