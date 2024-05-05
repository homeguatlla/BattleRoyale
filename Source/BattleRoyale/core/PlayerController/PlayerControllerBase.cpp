// Fill out your copyright notice in the Description page of Project Settings.
#include "PlayerControllerBase.h"

#include "MultiplayerSessionsSubsystem.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"


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
}

void APlayerControllerBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	CheckTimeSync(DeltaSeconds);
	if(IsLocalController())
	{
		CheckConnectivity();
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