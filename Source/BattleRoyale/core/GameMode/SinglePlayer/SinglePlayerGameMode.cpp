// Copyright Epic Games, Inc. All Rights Reserved.

#include "SinglePlayerGameMode.h"

#include "SinglePlayerConfigurationInfo.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"
#include "GameFramework/GameState.h"


ASinglePlayerGameMode::ASinglePlayerGameMode()
	: Super()
{
	DefaultPawnClass = CharacterClass;

	//TODO igual hace falta un nuevo hud para single player, o
	//dividir el battleroyalehud en single y multi
	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();
}

void ASinglePlayerGameMode::DoInitGame()
{
}

void ASinglePlayerGameMode::DoGenereicPlayerInitialization(AController* controller)
{
}

float ASinglePlayerGameMode::GetOnceMatchEndedTimeToResetSessionTime() const
{
	return GetConfigurationInfo()->GetOnceMatchEndedTimeToResetSessionTime();
}

USinglePlayerConfigurationInfo* ASinglePlayerGameMode::GetConfigurationInfo() const
{
	check(ConfigurationInfo);
	return Cast<USinglePlayerConfigurationInfo>(ConfigurationInfo);
}
