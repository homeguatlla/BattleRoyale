// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <memory>

#include "CoreMinimal.h"
#include "IPlayerState.h"
#include "UObject/Interface.h"
#include "IGameState.generated.h"


class UGameModeConfigurationInfo;
class UGameRules;

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIGameState : public UInterface
{
	GENERATED_BODY()
};


class BATTLEROYALE_API IIGameState
{
	GENERATED_BODY()

public:
	virtual bool AreAllPlayersReplicated() const = 0;
	virtual void StartGameServer() = 0;
	virtual bool HasGameStarted() const = 0;
	virtual bool IsGameReadyToStart() const = 0;
	virtual bool HasMatchEnded() const = 0;
	
	virtual int GetNumPlayers() const = 0;
	virtual int GetNumTeams() const = 0 ;
	virtual void PerformActionForEachPlayerState(std::function<bool(IIPlayerState* playerState)> action) const = 0;

	virtual void SetWinnerTeam(int teamId) = 0;
	virtual int GetWinnerTeam() const = 0;
	virtual void MatchEndServer() = 0;
	virtual void CloseAllPlayersGameSessionServer() const = 0;
	
	UFUNCTION(BlueprintCallable)
	virtual void PlayerInteraction(const APlayerController* playerController, const FString& action) = 0;
	virtual void NotifyGameModeConfigurationInfo(const UGameModeConfigurationInfo* configurationInfo) = 0;
};
