// Fill out your copyright notice in the Description page of Project Settings.


#include "EndOfGameRule.h"

#include "ShowStatsScreenRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void EndOfGameRule::Initialize(UWorld* world, IIGameState* gameState)
{
	mWorld = world;
	mGameState = gameState;
}

bool EndOfGameRule::Evaluate()
{
	return true;
}

bool EndOfGameRule::Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule EndOfGameRule"));

	rules.clear();
	
	//TODO Maybe notify the winner and the end of game to the other players alive
	mGameState->NotifyAnnouncementOfWinner();

	//add new rules
	const auto showStatsScreenRule = std::make_shared<ShowStatsScreenRule>();
	showStatsScreenRule->Initialize(mWorld, mGameState);
	rules.push_back(showStatsScreenRule);
	
	return true;
}
