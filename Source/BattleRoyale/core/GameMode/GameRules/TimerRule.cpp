// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerRule.h"

#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"


void UTimerRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mIsTimerOver = false;
	mDuration = 0;
}

void UTimerRule::BeginDestroy()
{
	GetWorld()->GetTimerManager().ClearTimer(mTimerHandle);
	
	UObject::BeginDestroy();
}

bool UTimerRule::Evaluate()
{
	return mIsTimerOver;
}

bool UTimerRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule UTimerRule"));
	
	return false;
}


void UTimerRule::Start()
{
	GetWorld()->GetTimerManager().SetTimer(mTimerHandle, this, &UTimerRule::OnTimerFinished, mDuration, true);
}

void UTimerRule::OnTimerFinished()
{
	mIsTimerOver = true;
}
