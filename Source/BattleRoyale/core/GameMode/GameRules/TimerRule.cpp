// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerRule.h"

#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"


void TimerRule::Initialize(UWorld* world, IIGameState* gameState)
{
	mWorld = world;
	mGameState = gameState;
	mIsTimerOver = false;
	mDuration = 0;
}

bool TimerRule::Evaluate()
{
	return mIsTimerOver;
}

bool TimerRule::Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule UTimerRule"));
	
	return false;
}


void TimerRule::Start()
{
	FTimerDelegate timerDelegate;
	timerDelegate.BindLambda([this] {
		OnTimerFinished();
	});
	
	mWorld->GetTimerManager().SetTimer(mTimerHandle, timerDelegate, mDuration, true);
}

void TimerRule::OnTimerFinished()
{
	mIsTimerOver = true;
	mWorld->GetTimerManager().ClearTimer(mTimerHandle);
}
