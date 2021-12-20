// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"
#include "UObject/ConstructorHelpers.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Core/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();
}
