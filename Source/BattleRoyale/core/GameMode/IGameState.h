// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <functional>
#include <memory>

#include "CoreMinimal.h"
#include "IPlayerState.h"
#include "UObject/Interface.h"
#include "IGameState.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIGameState : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGameState")
	virtual bool DidCountdownStart() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGameState")
	virtual bool DidCountdownFinish() const = 0;

	virtual void StartGameServer() = 0;
	virtual bool HasGameStarted() const = 0;
	
	virtual void StartCountdownServer(int duration) = 0;
	virtual int GetNumPlayers() const = 0;
	virtual int GetNumTeams() const = 0 ;
	virtual void PerformActionForEachPlayerState(std::function<bool(const IIPlayerState* playerState)> action) const = 0;

	virtual void SetWinnerTeam(int teamId) = 0;
	virtual int GetWinnerTeam() const = 0;
};
