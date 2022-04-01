// Fill out your copyright notice in the Description page of Project Settings.


#include "EndOfGameRule.h"
#include "BattleRoyale/core/GameMode/IGameState.h"


void UEndOfGameRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UEndOfGameRule::Evaluate()
{
	return true;
}

bool UEndOfGameRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	//TODO apply end of game rule
	//Maybe notify the winner and the end of game to the other players alive
	return false;
}
