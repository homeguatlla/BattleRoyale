// Fill out your copyright notice in the Description page of Project Settings.


#include "StartGameRule.h"

#include "CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/core/GameMode/IGameState.h"


void UStartGameRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UStartGameRule::Evaluate()
{
	return mGameState->DidCountdownStart() && mGameState->DidCountdownFinish() && mGameState->GetNumTeams() > 1;
}

bool UStartGameRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	//TODO hay que ver como hacemos esto, pero lo lógico sería informar al game mode para que la partida empiece.
	//SetGameStarted o algo así.
	
	rules.Empty();
	
	//Adding new rules
	const auto checkThereIsOnlyOneTeamAliveRule = NewObject<UCheckThereIsOnlyOneTeamAliveRule>();
	checkThereIsOnlyOneTeamAliveRule->Initialize(mGameState);
	rules.Add(checkThereIsOnlyOneTeamAliveRule);
	
	return true;
}
