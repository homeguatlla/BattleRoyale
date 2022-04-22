// Fill out your copyright notice in the Description page of Project Settings.


#include "TimerRule.h"

#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"


void TimerRule::Initialize(IIGameState* gameState)
{
	mIsTimerOver = false;
	mDuration = 0;
}
/*
void TimerRule::BeginDestroy()
{
	//GetWorld()->GetTimerManager().ClearTimer(mTimerHandle);
	
	UObject::BeginDestroy();
}*/

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
	//GetWorld()->GetTimerManager().SetTimer(mTimerHandle, this, &UTimerRule::OnTimerFinished, mDuration, true);
}

void TimerRule::OnTimerFinished()
{
	mIsTimerOver = true;
}
