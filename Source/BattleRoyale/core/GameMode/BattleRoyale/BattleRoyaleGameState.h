// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "FSM/BattleRoyaleContext.h"
#include "FSM/States/BattleRoyaleStates.h"
#include "BattleRoyaleGameState.generated.h"


UCLASS()
class BATTLEROYALE_API ABattleRoyaleGameState : public AGameState, public IIGameState
{
	GENERATED_BODY()
public:
	ABattleRoyaleGameState();
	
	virtual bool DidCountdownStart() const override{ return mDidCountdownStart; }
	virtual void StartCountdownServer(int duration) override;
	virtual bool DidCountdownFinish() const override { return mRemainingCounts <= 0; }

	virtual bool AreAllPlayersReplicated() const override;
	
	virtual void StartGameServer() override;
	virtual bool HasGameStarted() const override { return mHasGameStarted; }
	
	virtual int GetNumPlayers() const override { return PlayerArray.Num(); }
	virtual int GetNumTeams() const override;
	
	virtual void PerformActionForEachPlayerState(std::function<bool(const IIPlayerState* playerState)> action) const override;

	virtual void SetWinnerTeam(int teamId) override { mWinnerTeamId = teamId; }
	virtual int GetWinnerTeam() const override { return mWinnerTeamId; }
	virtual void NotifyAnnouncementOfWinner() const override;

	virtual void Tick(float DeltaSeconds) override;
	
private:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnCountdownFinishedServer();
	void CreateStatesMachine();
		
	UFUNCTION()
	void OnRep_RemainingCount() const;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGameStarted();

	bool mDidCountdownStart = false;
	FTimerHandle mCountdownTimerHandle;

	UPROPERTY(ReplicatedUsing=OnRep_RemainingCount)
	int mRemainingCounts;

	int mWinnerTeamId;
	bool mHasGameStarted = false;

	//States machine to control the game state
	StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext> mStatesMachineController;
	std::shared_ptr<BRModeFSM::BattleRoyaleContext> mGameStateFSMContext;
};
