// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckThereIsOnlyOneTeamAlive.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"


void UCheckThereIsOnlyOneTeamAlive::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UCheckThereIsOnlyOneTeamAlive::Evaluate() const
{
	if(mGameState->GetNumPlayers() <= 0)
	{
		return false;
	}
	
	int teamIdAlive = -1;
	bool thereIsOnlyOneTeamAlive = true;

	mGameState->PerformActionForEachPlayerState(
		[&teamIdAlive, &thereIsOnlyOneTeamAlive](const IIPlayerState* playerState)
		{
			if(playerState->IsAlive())
			{
				if(teamIdAlive >=0 && teamIdAlive != playerState->GetTeamId())
				{
					thereIsOnlyOneTeamAlive = false;
					return true; //finish the search
				}
				else
				{
					teamIdAlive = playerState->GetTeamId();
				}
			}
			return false;//continue searching
		});
	
	return thereIsOnlyOneTeamAlive;
}

void UCheckThereIsOnlyOneTeamAlive::Execute() const
{
	//TODO crear la regla de fin de partida
	//devolver las reglas nuevas a añadir o mejor que
	//reciba las reglas y añada directamente o quite si hace falta.
}
