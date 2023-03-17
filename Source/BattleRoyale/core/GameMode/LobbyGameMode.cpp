// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"

#include "IGameState.h"
#include "IMultiplayerGameState.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Configuration/MultiplayerConfigurationInfo.h"
#include "GameFramework/GameStateBase.h"


void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	if(const auto gameState = GetGameState())
	{
		if(gameState->GetNumPlayers() == mNumPlayers)
		{
			if(const auto world = GetWorld())
			{
				bUseSeamlessTravel = true;
				const FString url =	mConfigurationInfo->GetMapsPath().ToString() +
									utils::UtilsLibrary::GetValidMapName(GetWorld(), mConfigurationInfo->GetGameMapName()).ToString() +
									//"?game=/Game/ThirdPersonCPP/Blueprints/BP_MSGameMode.BP_MSGameMode_C" +
									"?listen";
		
				world->ServerTravel(url, true);	
			}
		}
	}
}

IIMultiplayerGameState* ALobbyGameMode::GetGameState() const
{
	if(GameState != nullptr && GameState->Implements<UIGameState>())
	{
		return Cast<IIMultiplayerGameState>(GameState);
	}
	return nullptr;
}
