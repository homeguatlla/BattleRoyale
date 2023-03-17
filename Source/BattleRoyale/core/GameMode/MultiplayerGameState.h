// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IMultiplayerGameState.h"
#include "GameFramework/GameState.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "BattleRoyale/FSM/BattleRoyaleContext.h"
#include "BattleRoyale/FSM/States/BattleRoyaleStates.h"

#include "MultiplayerGameState.generated.h"


UCLASS()
class BATTLEROYALE_API AMultiplayerGameState : public AGameState, public IIGameState, public IIMultiplayerGameState
{
	GENERATED_BODY()
public:
	AMultiplayerGameState();
	
	virtual bool AreAllPlayersReplicated() const override;

	virtual void StartGameServer() override;	
	virtual bool HasGameStarted() const override;
	virtual bool IsGameReadyToStart() const override;
	virtual bool HasMatchEnded() const override;
	
	virtual int GetNumPlayers() const override { return PlayerArray.Num(); }
	virtual int GetNumTeams() const override;
	
	virtual void PerformActionForEachPlayerState(std::function<bool(IIPlayerState* playerState)> action) const override;

	virtual void SetWinnerTeam(int teamId) override { mWinnerTeamId = teamId; }
	virtual int GetWinnerTeam() const override { return mWinnerTeamId; }
	virtual void MatchEndServer() override;
	virtual void CloseAllPlayersGameSessionServer() const override;
	
	UFUNCTION(BlueprintCallable)
	virtual void PlayerInteraction(const APlayerController* playerController, const FString& action) override;
	virtual void NotifyGameModeConfigurationInfo(const UGameModeConfigurationInfo* configurationInfo) override;
	virtual void NotifyNumTeamsAndPlayersAlive() override;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddStatesMachineServer(
		StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>& fsmController,
		std::shared_ptr<BRModeFSM::BattleRoyaleContext>& fsmContext) {};

	
protected:
	virtual void BeginPlay() override;
	
private:
	void NotifyMatchEndedServer() const;
	uint8 GetNumPlayersAlive() const;
	uint8 GetNumTeamsAlive() const;
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameStarted();

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastGameModeConfigurationInfo(const UGameModeConfigurationInfo* configurationInfo);
	
	
	int mWinnerTeamId;
	
	//States machine to control the game state
	StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext> mStatesMachineController;
	std::shared_ptr<BRModeFSM::BattleRoyaleContext> mGameStateFSMContext;
};
