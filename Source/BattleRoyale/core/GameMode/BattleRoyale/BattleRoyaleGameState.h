// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameState.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "FSM/BattleRoyaleContext.h"
#include "FSM/States/BattleRoyaleStates.h"
#include "BattleRoyaleGameState.generated.h"


UCLASS()
class BATTLEROYALE_API ABattleRoyaleGameState : public AMultiplayerGameState
{
	GENERATED_BODY()
public:
	ABattleRoyaleGameState();
	
	void StartCountdownServer(int duration);	
	virtual bool IsGameReadyToStart() const override;
	bool CanStartCountDown(uint8 numTeamsToStartCountDown) const;
	
private:
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnCountdownFinishedServer();
	virtual void AddStatesMachineServer(
		StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>& fsmController,
		std::shared_ptr<BRModeFSM::BattleRoyaleContext>& fsmContext) override;
	bool DidCountdownStart() const { return mDidCountdownStart; }
	bool DidCountdownFinish() const  { return mRemainingCounts <= 0; }
	
	UFUNCTION()
	void OnRep_RemainingCount() const;
	
	bool mDidCountdownStart = false;
	FTimerHandle mCountdownTimerHandle;

	UPROPERTY(ReplicatedUsing=OnRep_RemainingCount)
	int mRemainingCounts;
};
