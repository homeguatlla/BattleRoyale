// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameState.h"

#include <set>

#include "BattleRoyaleGameMode.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "GameFramework/PlayerState.h"
#include "Net/UnrealNetwork.h"


ABattleRoyaleGameState::ABattleRoyaleGameState() :
mRemainingCounts{0},
mWinnerTeamId{-1}

{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void ABattleRoyaleGameState::BeginPlay()
{
	Super::BeginPlay();
	if(HasAuthority())
	{
		CreateStatesMachineServer();
		SetActorTickEnabled(true);
	}
}

void ABattleRoyaleGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(HasAuthority())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, "ABattleRoyaleGameState::Tick!");
		mStatesMachineController.Update(DeltaSeconds);
	}
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
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		
	mRemainingCounts--; 
	if(mRemainingCounts <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(mCountdownTimerHandle);

		gameInstance->GetEventDispatcher()->OnFinishCountDown.Broadcast();

		UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished Countdown finished"));
	}
	else
	{
		UE_LOG(LogGameMode, Log, TEXT("ABattleRoyaleGameStateBase::OnCountdownFinished %d"), mRemainingCounts);
	}
	gameInstance->GetEventDispatcher()->OnRefreshCountDown.Broadcast(mRemainingCounts);
}

void ABattleRoyaleGameState::OnRep_RemainingCount() const
{
	//Este método no se ejecuta en servidor. Por eso hay que poner la llamada cuando se modifica
	//la variable replicada
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	
	gameInstance->GetEventDispatcher()->OnRefreshCountDown.Broadcast(mRemainingCounts);
	if(mRemainingCounts <= 0)
	{
		gameInstance->GetEventDispatcher()->OnFinishCountDown.Broadcast();
	}
}

void ABattleRoyaleGameState::MulticastGameStarted_Implementation()
{
	//To notify HUD
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnGameStarted.Broadcast();
}

void ABattleRoyaleGameState::CreateStatesMachineServer()
{
	auto gameMode = Cast<ABattleRoyaleGameMode>(GetWorld()->GetAuthGameMode());
	mGameStateFSMContext = std::make_shared<BRModeFSM::BattleRoyaleContext>(GetWorld(), this, gameMode->CountdownTimeToStartGame);
	
	BattleRoyale::StatesMachineFactory factory;
	
	mStatesMachineController.AddMachine(
		std::move(factory.CreateBattleRoyaleModeFSM(
			FSMType::BATTLEROYALE_GAMEMODE,
			mGameStateFSMContext)));
}