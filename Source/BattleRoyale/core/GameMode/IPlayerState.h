// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "PlayerState/FSM/States/PlayerStateStates.h"
#include "IPlayerState.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIPlayerState : public UAbilitySystemInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIPlayerState: public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentInterface() const = 0;
	virtual bool IsAlive() const = 0;
	virtual bool IsPawnReplicated() const = 0;
	
	virtual void SetTeamId(int teamId) = 0;
	virtual int GetTeamId() const = 0;
	
	virtual FString GetPlayerNickName() const = 0;

	virtual void AddKill() = 0;
	virtual int GetNumKills() const = 0;

	virtual void OnGameStarted() = 0;
	virtual void ShowStatsScreen() const = 0;
	virtual void ShowVictoryScreen() const = 0;
	virtual void ShowDeathScreen() const = 0;
	
	//TODO estos métodos probablemente deberían estar en un player state más específico del battle royale
	//o si el equipo ganador lo ponemos en un atributo de gas pues igual ya se podrá consultar de una manera
	//más genérica.
	virtual void SetAsWinner() = 0;
	virtual bool DidPlayerWin() const = 0;
	
	virtual void NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const = 0;
	virtual void NotifyNumKillsToSelf() = 0;
	virtual void NotifyGameOver(bool isWinner) = 0;
	virtual void ForceFSMStateClient(BRPlayerStateFSM::PlayerStateState state) = 0;
};
