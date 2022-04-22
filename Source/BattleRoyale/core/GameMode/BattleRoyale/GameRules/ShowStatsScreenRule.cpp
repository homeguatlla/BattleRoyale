// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowStatsScreenRule.h"

#include "CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void ShowStatsScreenRule::Initialize(UWorld* world, IIGameState* gameState)
{
	TimerRule::Initialize(world, gameState);

	SetDuration(5);
	Start();
}

bool ShowStatsScreenRule::Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule ShowStatsScreenRule"));

	rules.clear();

	//TODO mostrar la pantalla de stats.
	
	return false;
}
