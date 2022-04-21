// Fill out your copyright notice in the Description page of Project Settings.


#include "EndOfGameRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void EndOfGameRule::Initialize(IIGameState* gameState)
{
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
	
	return true;
}
