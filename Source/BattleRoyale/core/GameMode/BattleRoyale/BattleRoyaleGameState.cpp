// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameState.h"

#include <set>

#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"

ABattleRoyaleGameState::ABattleRoyaleGameState() :
mRemainingCounts{0},
mWinnerTeamId{-1}

{
}

void ABattleRoyaleGameState::StartCountdownServer(int duration)
{
	mDidCountdownStart = true;

	mRemainingCounts = duration;
	if(HasAuthority())
	{
		GetWorld()->GetTimerManager().SetTimer(mCountdownTimerHandle, this, &ABattleRoyaleGameState::OnCountdownFinishedServer, 1, true);
	}
}

bool ABattleRoyaleGameState::AreAllPlayersReplicated() const
{
	bool AreAllPlayerStatesWithPawn = true;
	PerformActionForEachPlayerState(
		[&AreAllPlayerStatesWithPawn](const IIPlayerState* playerState)
		{
			AreAllPlayerStatesWithPawn &= playerState->IsPawnReplicated();
			return false;
		});

	return AreAllPlayerStatesWithPawn;
}

void ABattleRoyaleGameState::StartGameServer()
{
	mHasGameStarted = true;
	mWinnerTeamId = -1;
	MulticastGameStarted();
}

int ABattleRoyaleGameState::GetNumTeams() const
{
	std::set<int> teams;

	PerformActionForEachPlayerState(
		[&teams](const IIPlayerState* playerState) -> bool
		{
			teams.insert(playerState->GetTeamId());
			return false;
		});
	
	return teams.size();
}

void ABattleRoyaleGameState::PerformActionForEachPlayerState(
	std::function<bool(const IIPlayerState* playerState)> action) const
{
	for(const auto playerState : PlayerArray)
	{
		if(playerState->Implements<UIPlayerState>())
		{
			const auto specificPlayerState = Cast<IIPlayerState>(playerState);
			if(action(specificPlayerState))
				break;
		}
	}
}

void ABattleRoyaleGameState::NotifyAnnouncementOfWinner() const
{
	PerformActionForEachPlayerState(
		[&](const IIPlayerState* playerState) -> bool
		{
			if(playerState->GetTeamId() == GetWinnerTeam())
			{
				playerState->NotifyAnnouncementOfWinner();
			}
			return false;
		});
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
		UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished Countdown finished"));
	}
	else
	{
		UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished %d"), mRemainingCounts);
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

void ABattleRoyaleGameState::MulticastGameStarted_Implementation()
{
	//To notify the Menu UI
	OnGameStartedForMenu.Broadcast();

	//To notify HUD
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnGameStarted.Broadcast();
}
