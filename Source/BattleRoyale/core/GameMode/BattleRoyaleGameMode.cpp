// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"

ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	DefaultPawnClass = mCharacter;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();
}
