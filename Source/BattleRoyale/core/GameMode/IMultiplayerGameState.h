// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>

#include "CoreMinimal.h"
#include "IGameState.h"
#include "IPlayerState.h"
#include "IMultiplayerGameState.generated.h"


class UGameModeConfigurationInfo;
class UGameRules;

// This class does not need to be modified.
UINTERFACE(NotBlueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIMultiplayerGameState : public UIGameState
{
	GENERATED_BODY()
};


class BATTLEROYALE_API IIMultiplayerGameState
{
	GENERATED_BODY()

public:
	virtual bool AreAllPlayersReplicated() const = 0;
	
	virtual int GetNumPlayers() const = 0;
	virtual int GetNumTeams() const = 0 ;
	virtual void PerformActionForEachPlayerState(std::function<bool(IIPlayerState* playerState)> action) const = 0;

	virtual void SetWinnerTeam(int teamId) = 0;
	virtual int GetWinnerTeam() const = 0;
	virtual void CloseAllPlayersGameSessionServer() const = 0;
	
	virtual void NotifyNumTeamsAndPlayersAlive() = 0;
};
