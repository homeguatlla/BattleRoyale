// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameState.h"

#include <set>

#include "BattleRoyaleGameMode.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "Net/UnrealNetwork.h"


ABattleRoyaleGameState::ABattleRoyaleGameState() :
mRemainingCounts{0}
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

bool ABattleRoyaleGameState::IsGameReadyToStart() const
{
	return DidCountdownStart() && DidCountdownFinish();
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

void ABattleRoyaleGameState::AddStatesMachineServer(
	StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>& fsmController,
	std::shared_ptr<BRModeFSM::BattleRoyaleContext>& fsmContext)
{
	const auto gameMode = Cast<ABattleRoyaleGameMode>(GetWorld()->GetAuthGameMode());
	fsmContext = std::make_shared<BRModeFSM::BattleRoyaleContext>(GetWorld(), this, gameMode->CountdownTimeToStartGame);
	
	BattleRoyale::StatesMachineFactory factory;
	
	fsmController.AddMachine(
		std::move(factory.CreateModeFSM(
			FSMType::BATTLEROYALE_GAMEMODE,
			fsmContext)));
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
