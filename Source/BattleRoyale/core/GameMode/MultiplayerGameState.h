// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "BattleRoyale/FSM/BattleRoyaleContext.h"
#include "BattleRoyale/FSM/States/BattleRoyaleStates.h"

#include "MultiplayerGameState.generated.h"


UCLASS()
class BATTLEROYALE_API AMultiplayerGameState : public AGameState, public IIGameState
{
	GENERATED_BODY()
public:
	AMultiplayerGameState();
	
	virtual bool AreAllPlayersReplicated() const override;

	virtual void StartGameServer() override;
	virtual bool HasGameStarted() const override;
	virtual bool IsGameReadyToStart() const override;
	
	virtual int GetNumPlayers() const override { return PlayerArray.Num(); }
	virtual int GetNumTeams() const override;
	
	virtual void PerformActionForEachPlayerState(std::function<bool(const IIPlayerState* playerState)> action) const override;

	virtual void SetWinnerTeam(int teamId) override { mWinnerTeamId = teamId; }
	virtual int GetWinnerTeam() const override { return mWinnerTeamId; }
	virtual void NotifyAnnouncementOfWinner() const override;
	virtual void NotifyGameOver() const override;

	virtual float GetDurationInDeadState() const override { return DurationInDeadState; }
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void AddStatesMachineServer(
		StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>& fsmController,
		std::shared_ptr<BRModeFSM::BattleRoyaleContext>& fsmContext) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Gameplay")
	float DurationInDeadState = 5;
	
protected:
	virtual void BeginPlay() override;
	
private:
	
	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameStarted();
	
	int mWinnerTeamId;
	
	//States machine to control the game state
	StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext> mStatesMachineController;
	std::shared_ptr<BRModeFSM::BattleRoyaleContext> mGameStateFSMContext;
};
