// Fill out your copyright notice in the Description page of Project Settings.


#include "GameFinishedRule.h"

#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void GameFinishedRule::Initialize(UWorld* world, IIGameState* gameState)
{
	mWorld = world;
	mGameState = gameState;
}

bool GameFinishedRule::Evaluate()
{
	return true;
}

bool GameFinishedRule::Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule EndOfGameRule"));

	rules.clear();
	
	mGameState->MatchEndServer();
	
	return true;
}
