// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameState.h"

#include "Net/UnrealNetwork.h"

ABattleRoyaleGameState::ABattleRoyaleGameState() :
mRemainingCounts{0}

{
}

void ABattleRoyaleGameState::StartCountdownServer(uint8 duration)
{
	mDidCountdownStart = true;

	mRemainingCounts = duration;
	if(HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(mCountdownTimerHandle, this, &ABattleRoyaleGameState::OnCountdownFinishedServer, 1, true);
	}
}

void ABattleRoyaleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleRoyaleGameState, mRemainingCounts);
}

void ABattleRoyaleGameState::OnCountdownFinishedServer()
{
	mRemainingCounts--;
	if(mRemainingCounts <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(mCountdownTimerHandle);
		OnFinishCountDownDelegate.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished Countdown finished"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished %d"), mRemainingCounts);
	}
	OnRefreshCountDownDelegate.Broadcast(mRemainingCounts);
}

void ABattleRoyaleGameState::OnRep_RemainingCount() const
{
	//Este método no se ejecuta en servidor. Por eso hay que poner la llamada cuando se modifica
	//la variable replicada
	
	OnRefreshCountDownDelegate.Broadcast(mRemainingCounts);
	if(mRemainingCounts <= 0)
	{
		OnFinishCountDownDelegate.Broadcast();
	}
}