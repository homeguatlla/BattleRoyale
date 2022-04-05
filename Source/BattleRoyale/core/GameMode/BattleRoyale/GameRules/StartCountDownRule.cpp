// Fill out your copyright notice in the Description page of Project Settings.


#include "StartCountDownRule.h"

#include "StartGameRule.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "GameFramework/GameModeBase.h"

void UStartCountDownRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UStartCountDownRule::Evaluate()
{
	return !mGameState->DidCountdownStart() && mGameState->GetNumTeams() > 1;
}

bool UStartCountDownRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	UE_LOG(LogGameMode, Display, TEXT("GameRules: Executing Rule StartCountDownRule"));

	const auto startGameRule = NewObject<UStartGameRule>();	
	startGameRule->Initialize(mGameState);
	rules.Add(startGameRule);
	
	mGameState->StartCountdownServer(mCountdownTimeToStartGame);
	
	return true;
}
