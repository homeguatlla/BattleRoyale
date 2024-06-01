// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameState.h"
#include "BattleRoyaleGameMode.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "Net/UnrealNetwork.h"


ABattleRoyaleGameState::ABattleRoyaleGameState() :
mCountDownTime{0}, mInitialCountdownTime{0}
{
}

void ABattleRoyaleGameState::StartCountdownServer(int duration)
{
	mDidCountdownStart = true;
	mInitialCountdownTime = duration + GetServerWorldTimeSeconds();
	mCountDownTime = mInitialCountdownTime;
	
	//Refreshing initial countdown
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnRefreshCountDown.Broadcast(mCountDownTime);
}

bool ABattleRoyaleGameState::IsGameReadyToStart() const
{
	return DidCountdownStart() && DidCountdownFinish();
}

bool ABattleRoyaleGameState::CanStartCountDown(uint8 numTeamsToStartCountDown) const
{
	return !DidCountdownStart() && GetNumTeams() >= numTeamsToStartCountDown;
}

void ABattleRoyaleGameState::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if(GetNetMode() == NM_Client)
	{
		int kk = 0;
	}
	if(DidCountdownStart())
	{
		UpdateCountDownTime(DeltaSeconds);
		CheckIfCountDownFinished();
	}
}

void ABattleRoyaleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ABattleRoyaleGameState, mInitialCountdownTime);
}

void ABattleRoyaleGameState::AddStatesMachineServer(
	StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>& fsmController,
	std::shared_ptr<BRModeFSM::BattleRoyaleContext>& fsmContext)
{
	const auto gameMode = Cast<ABattleRoyaleGameMode>(GetWorld()->GetAuthGameMode());
	fsmContext = std::make_shared<BRModeFSM::BattleRoyaleContext>(
		GetWorld(),
		gameMode,
		this,
		gameMode->GetCountdownTimeToStartGame(),
		gameMode->GetNumTeamsToStartCountDown());
	
	BattleRoyale::StatesMachineFactory factory;
	
	fsmController.AddMachine(
		std::move(factory.CreateModeFSMBattleRoyale(
			FSMType::BATTLEROYALE_GAMEMODE,
			fsmContext)));
}

void ABattleRoyaleGameState::UpdateCountDownTime(float delta_seconds)
{
	const auto serverWorldTimeSeconds = GetServerWorldTimeSeconds();
	const auto secondsLeft = FMath::CeilToInt(mCountDownTime - serverWorldTimeSeconds);
	
	//if(mRemainingSeconds != secondsLeft)
	{
		//Has passed one second: update UI
		const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		auto remainingTime = FMath::Max(0.0f, mCountDownTime - serverWorldTimeSeconds );
		//mInitialCountdownTime -= serverWorldTimeSeconds;
		UE_LOG(LogGameState, Log, TEXT("ABattleRoyaleGameState::UpdateCountDownTime Countdown %f"), remainingTime);
		gameInstance->GetEventDispatcher()->OnRefreshCountDown.Broadcast(remainingTime);
	}
	mRemainingSeconds = secondsLeft;
}

void ABattleRoyaleGameState::CheckIfCountDownFinished()
{
	if(mRemainingSeconds <= 0)
	{
		const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		gameInstance->GetEventDispatcher()->OnFinishCountDown.Broadcast();
	}
}

void ABattleRoyaleGameState::OnRep_InitialCountdownTime()
{
	//Lo que tenemos que hacer es actualizar el timer en el playercontroller.
	//Cuando se crea el player controller este pilla el gamestate y inicializa el countdowntime
	//y el game state no tiene que actualizar nada, solo iniciar y ver cuando termina.
	if(!mDidCountdownStart)
	{
		mDidCountdownStart = true;
		mCountDownTime = mInitialCountdownTime;
	}
}
