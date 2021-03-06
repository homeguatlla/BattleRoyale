// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleGameMode.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/HUD/BattleRoyaleHUD.h"
#include "BattleRoyale/core/Utils/TeamSelectionStrategies/SimpleTeamSelectionStrategy.h"
#include "GameFramework/GameState.h"


ABattleRoyaleGameMode::ABattleRoyaleGameMode()
	: Super()
{
	DefaultPawnClass = CharacterClass;

	// use our custom HUD class
	HUDClass = ABattleRoyaleHUD::StaticClass();
}

void ABattleRoyaleGameMode::DoInitGame()
{
	InitializeTeamSelectionStrategy();
}

void ABattleRoyaleGameMode::DoGenereicPlayerInitialization(AController* controller)
{
	ApplyTeamSelectionStrategy(controller);
}

float ABattleRoyaleGameMode::GetOnceMatchEndedTimeToResetSessionTime() const
{
	return GetConfigurationInfo()->GetOnceMatchEndedTimeToResetSessionTime();
}

uint8 ABattleRoyaleGameMode::GetNumPlayersPerTeam() const
{
	return GetConfigurationInfo()->GetNumPlayersPerTeam();
}

float ABattleRoyaleGameMode::GetCountdownTimeToStartGame() const
{
	return GetConfigurationInfo()->GetCountdownTimeToStartGame();
}

void ABattleRoyaleGameMode::ApplyTeamSelectionStrategy(const AController* controller) const
{
	if(mTeamSelectionStrategy != nullptr)
	{
		const auto playerState = controller->GetPlayerState<APlayerStateBase>();
		if(playerState && playerState->Implements<UIPlayerState>())
		{
			const auto playerStateInterface = Cast<IIPlayerState>(playerState);
			const auto teamId = mTeamSelectionStrategy->GetNextTeamId();
			UE_LOG(LogGameMode, Display, TEXT("ABattleRoyaleGameMode::ApplyTeamSelectionStrategy Team id: %d"), teamId);
			playerStateInterface->SetTeamId(teamId);
		}
	}
}

UBattleRoyaleConfigurationInfo* ABattleRoyaleGameMode::GetConfigurationInfo() const
{
	return Cast<UBattleRoyaleConfigurationInfo>(ConfigurationInfo);
}

void ABattleRoyaleGameMode::InitializeTeamSelectionStrategy()
{
	mTeamSelectionStrategy = NewObject<USimpleTeamSelectionStrategy>();
	mTeamSelectionStrategy->Initialize(GetNumPlayersPerTeam());
}

