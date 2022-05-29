// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerGameState.h"

#include <set>

#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "GameFramework/GameMode.h"
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

void AMultiplayerGameState::PlayerInteraction(const APlayerController* playerController, const FString& action)
{
	if(playerController != nullptr)
	{
		const auto playerState = playerController->GetPlayerState<IIPlayerState>();
		if(playerState != nullptr)
		{
			playerState->PlayerInteraction(action);
		}
	}
}

void AMultiplayerGameState::NotifyGameModeConfigurationInfo(const UGameModeConfigurationInfo* configurationInfo)
{
	MulticastGameModeConfigurationInfo(configurationInfo);
}

void AMultiplayerGameState::NotifyNumTeamsAndPlayersAlive()
{
	uint8 numTeamsAlive = GetNumTeamsAlive();
	uint8 numPlayersAlive = GetNumPlayersAlive();
	PerformActionForEachPlayerState([&numTeamsAlive, &numPlayersAlive](IIPlayerState* playerState)
		{
			playerState->NotifyNumTeamsAndPlayersAlive(numTeamsAlive, numPlayersAlive);
			return false;
		});
}

void AMultiplayerGameState::MulticastGameModeConfigurationInfo_Implementation(
	const UGameModeConfigurationInfo* configurationInfo)
{
	PerformActionForEachPlayerState([&configurationInfo](IIPlayerState* playerState)
		{
			playerState->Initialize(configurationInfo);
			return false;
		});
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
	NotifyNumTeamsAndPlayersAlive();
}

void AMultiplayerGameState::MatchEndServer()
{
	//Setting unreal state
	SetMatchState(MatchState::WaitingPostMatch);
	NotifyMatchEndedServer();
}

void AMultiplayerGameState::CloseAllPlayersGameSessionServer() const
{
	PerformActionForEachPlayerState(
		[](IIPlayerState* playerState) -> bool
		{
			playerState->Restart();
			return false;
		});	
}

bool AMultiplayerGameState::HasGameStarted() const
{
	return mStatesMachineController.GetCurrentStateID(static_cast<int>(FSMType::BATTLEROYALE_GAMEMODE)) == BRModeFSM::BattleRoyaleState::STATE_GAMELOOP;
}

bool AMultiplayerGameState::IsGameReadyToStart() const
{
	return true;
}

bool AMultiplayerGameState::HasMatchEnded() const
{
	return AGameState::HasMatchEnded();
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
	std::function<bool(IIPlayerState* playerState)> action) const
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

void AMultiplayerGameState::NotifyMatchEndedServer() const
{
	PerformActionForEachPlayerState(
		[&](IIPlayerState* playerState) -> bool
		{
			playerState->NotifyGameOverServer(true, playerState->GetTeamId() == GetWinnerTeam());
			return false;
		});
}

uint8 AMultiplayerGameState::GetNumPlayersAlive() const
{
	uint8 numPlayersAlive = 0;
	
	PerformActionForEachPlayerState(
		[this, &numPlayersAlive](const IIPlayerState* playerState)
		{
			if(playerState->IsAlive())
			{
				numPlayersAlive++;
			}
			return false;
		});

	return numPlayersAlive;
}

uint8 AMultiplayerGameState::GetNumTeamsAlive() const
{
	std::set<int> teams;
	
	PerformActionForEachPlayerState(
			[this, &teams](const IIPlayerState* playerState)
			{
				if(playerState->IsAlive())
				{
					teams.insert(playerState->GetTeamId());
				}
				return false;
			});
	
	return teams.size();
}

void AMultiplayerGameState::MulticastGameStarted_Implementation()
{
	//To notify HUD
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnGameStarted.Broadcast();
	
	//To notify all playerStates about gameStarted
	//This way we give the opportunity to the player state to initiate
	//once the server pawn is already spawned.
	PerformActionForEachPlayerState(
		[](IIPlayerState* playerState) -> bool
		{
			playerState->OnGameStarted();
			return false;
		});
}
