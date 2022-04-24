// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameState.h"

#include <set>

#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


AMultiplayerGameState::AMultiplayerGameState() :
mWinnerTeamId{-1}
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void AMultiplayerGameState::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		AddStatesMachineServer(mStatesMachineController, mGameStateFSMContext);
		SetActorTickEnabled(true);
	}
}

void AMultiplayerGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "ABattleRoyaleGameState::Tick!");
		mStatesMachineController.Update(DeltaSeconds);
	}
}

bool AMultiplayerGameState::AreAllPlayersReplicated() const
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

void AMultiplayerGameState::StartGameServer()
{
	mWinnerTeamId = -1;
	MulticastGameStarted();
}

bool AMultiplayerGameState::HasGameStarted() const
{
	return mStatesMachineController.GetCurrentStateID(static_cast<int>(FSMType::BATTLEROYALE_GAMEMODE)) == BRModeFSM::BattleRoyaleState::STATE_GAMELOOP;
}

bool AMultiplayerGameState::IsGameReadyToStart() const
{
	return true;
}

int AMultiplayerGameState::GetNumTeams() const
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

void AMultiplayerGameState::PerformActionForEachPlayerState(
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

void AMultiplayerGameState::NotifyAnnouncementOfWinner() const
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

void AMultiplayerGameState::NotifyGameOver() const
{
	PerformActionForEachPlayerState(
		[&](const IIPlayerState* playerState) -> bool
		{
			if(playerState->GetTeamId() == GetWinnerTeam())
			{
				playerState->NotifyGameOver();
			}
			return false;
		});
}

void AMultiplayerGameState::MulticastGameStarted_Implementation()
{
	//To notify HUD
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnGameStarted.Broadcast();
}
