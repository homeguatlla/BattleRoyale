// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerControllerBase.h"

#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"


class UMultiplayerSessionsSubsystem;

void APlayerControllerBase::ClientEnableInput_Implementation(bool enable)
{
	SetIgnoreLookInput(!enable);
	SetIgnoreMoveInput(!enable);
}

void APlayerControllerBase::EnableInput(bool enable)
{
	ClientEnableInput(enable);
}

float APlayerControllerBase::GetServerTime() const
{
	const auto currentTime = GetWorld()->GetTimeSeconds();
	
	if(HasAuthority())
	{
		return currentTime;
	}
	else
	{
		return currentTime + mClientServerDelta;
	}
}

void APlayerControllerBase::ReceivedPlayer()
{
	Super::ReceivedPlayer();

	if(IsLocalController())
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();

	const auto gameState = UGameplayStatics::GetGameState(GetWorld());
	check(gameState);
	mGameState = Cast<ABattleRoyaleGameState>(gameState);
}

void APlayerControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	CheckTimeSync(DeltaSeconds);
	if(IsLocalController())
	{
		CheckConnectivity();
	}

	if(HasAuthority())
	{
		return;
	}
	
	if(mGameState && mGameState->DidCountdownStart())
	{
		if(mCountDownTime == 0)
		{
			mCountDownTime = mGameState->GetCountDownTimeLeft();
		}
		UpdateCountDownTime(DeltaSeconds);
	}
}

void APlayerControllerBase::CheckTimeSync(float DeltaSeconds)
{
	mTimeSyncRunningTime += DeltaSeconds;
	if(IsLocalController() && mTimeSyncRunningTime > mTimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		mTimeSyncRunningTime = 0.0f;
	}
}

void APlayerControllerBase::CheckConnectivity() const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	check(gameInstance);

	const auto playerState = GetPlayerState<APlayerStateBase>();
	if(!playerState)
	{
		return;
	}

	const auto ping = playerState->GetPingInMilliseconds();

	const auto eventDispatcher = gameInstance->GetEventDispatcher();
	check(eventDispatcher);
	
	eventDispatcher->OnShowConnectivity.Broadcast(ping);
}

void APlayerControllerBase::UpdateCountDownTime(float deltaSeconds)
{
	const auto serverWorldTimeSeconds = mGameState->GetServerWorldTimeSeconds();
	const auto secondsLeft = FMath::CeilToInt(mCountDownTime - serverWorldTimeSeconds);

	UE_LOG(LogGameState, Log, TEXT("APlayerControllerBase::UpdateCountDownTime Countdown %f"), mCountDownTime - serverWorldTimeSeconds);
	if(mRemainingSeconds != secondsLeft)
	{
		//Has passed one second: update UI
		const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
		auto remainingTime = FMath::Max(0.0f, mCountDownTime - serverWorldTimeSeconds );
		mInitialCountdownTime -= serverWorldTimeSeconds;
		gameInstance->GetEventDispatcher()->OnRefreshCountDown.Broadcast(remainingTime);
	}
	mRemainingSeconds = secondsLeft;
}

void APlayerControllerBase::ServerRequestServerTime_Implementation(float timeOfClientRequest)
{
	const auto serverTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(timeOfClientRequest, serverTimeOfReceipt);
}

void APlayerControllerBase::ClientReportServerTime_Implementation(float timeOfClientRequest,
	float timeServerReceivedClientRequest)
{
	const auto currentTime = GetWorld()->GetTimeSeconds();
	const auto roundTripTime = currentTime - timeOfClientRequest;
	const auto currentServerTime = timeServerReceivedClientRequest + 0.5f * roundTripTime;
	mClientServerDelta = currentServerTime - currentTime;
}