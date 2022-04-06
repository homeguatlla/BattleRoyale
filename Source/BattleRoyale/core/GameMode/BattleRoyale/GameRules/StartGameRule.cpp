// Fill out your copyright notice in the Description page of Project Settings.


#include "StartGameRule.h"

#include "CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"


void UStartGameRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UStartGameRule::Evaluate()
{
	return mGameState->DidCountdownStart() && mGameState->DidCountdownFinish();
}

bool UStartGameRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Display, TEXT("GameRules: Executing Rule StartGameRule"));
	
	mGameState->StartGameServer();
	
	rules.Empty();
	
	//Adding new rules
	const auto checkThereIsOnlyOneTeamAliveRule = NewObject<UCheckThereIsOnlyOneTeamAliveRule>();
	checkThereIsOnlyOneTeamAliveRule->Initialize(mGameState);
	rules.Add(checkThereIsOnlyOneTeamAliveRule);
	
	return true;
}