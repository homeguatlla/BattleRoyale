// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameState.h"

ABattleRoyaleGameState::ABattleRoyaleGameState() :
mRemainingCounts{0}

{
}

void ABattleRoyaleGameState::StartCountdown(uint8 duration)
{
	mDidCountdownStart = true;

	mRemainingCounts = duration;
	GetWorld()->GetTimerManager().SetTimer(mCountdownTimerHandle, this, &ABattleRoyaleGameState::OnCountdownFinished, 1, true);
}

void ABattleRoyaleGameState::OnCountdownFinished()
{
	mRemainingCounts--;
	if(mRemainingCounts <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(mCountdownTimerHandle);
		UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished Countdown finished"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished %d"), mRemainingCounts);
	}
}