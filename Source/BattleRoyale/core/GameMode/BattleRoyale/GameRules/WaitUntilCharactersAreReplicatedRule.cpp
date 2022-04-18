// Fill out your copyright notice in the Description page of Project Settings.


#include "WaitUntilCharactersAreReplicatedRule.h"

#include "CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void UWaitUntilCharactersAreReplicatedRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UWaitUntilCharactersAreReplicatedRule::Evaluate()
{
	return mGameState->AreAllPlayersReplicated();
}

bool UWaitUntilCharactersAreReplicatedRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule WaitUntilCharactersAreReplicatedRule"));

	rules.Empty();

	//Adding new rules
	const auto checkThereIsOnlyOneTeamAliveRule = NewObject<UCheckThereIsOnlyOneTeamAliveRule>();
	checkThereIsOnlyOneTeamAliveRule->Initialize(mGameState);
	rules.Add(checkThereIsOnlyOneTeamAliveRule);
	
	return true;
}
