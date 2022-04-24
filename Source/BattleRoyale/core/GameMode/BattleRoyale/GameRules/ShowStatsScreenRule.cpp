// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowStatsScreenRule.h"

#include "CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void ShowStatsScreenRule::Initialize(UWorld* world, IIGameState* gameState)
{
	TimerRule::Initialize(world, gameState);

	SetDuration(gameState->GetDurationInDeadState());
	Start();
}

bool ShowStatsScreenRule::Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule ShowStatsScreenRule"));

	rules.clear();

	//Here, the game has finished so, we are gonna show the stats to all players still alive (the ones who won)
	//Pensar si puede suceder que un jugador que acaba de morir, se queda viendo durante 5 segundos la pantalla
	//de juego mientras los otros jugadores ya estan viendo las estadísticas? igual no es posible
	mGameState->NotifyGameOver();
	
	return false;
}
