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
	bool DidCountdownStart() const { return mDidCountdownStart; }
	float GetCountDownTimeLeft() const { return mCountDownTime; }
	
private:
	virtual void Tick(float DeltaSeconds) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual void AddStatesMachineServer(
		StatesMachineController<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext>& fsmController,
		std::shared_ptr<BRModeFSM::BattleRoyaleContext>& fsmContext) override;
	
	bool DidCountdownFinish() const  { return mRemainingSeconds <= 0; }
	void UpdateCountDownTime(float delta_seconds);
	void CheckIfCountDownFinished();

	UFUNCTION()
	void OnRep_InitialCountdownTime();
	bool mDidCountdownStart = false;
	
	
	float mCountDownTime;
	int mRemainingSeconds = 0;
	UPROPERTY(ReplicatedUsing=OnRep_InitialCountdownTime)
	int mInitialCountdownTime;
};
