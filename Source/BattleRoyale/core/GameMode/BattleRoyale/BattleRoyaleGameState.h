// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "GameFramework/GameState.h"
#include "BattleRoyaleGameState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API ABattleRoyaleGameState : public AGameState, public IIGameState
{
	GENERATED_BODY()
public:
	ABattleRoyaleGameState();
	virtual bool DidCountdownStart() const override{ return mDidCountdownStart; }
	virtual void StartCountdown(uint8 duration) override;
	virtual bool DidCountdownFinish() const override { return mRemainingCounts <= 0; }
private:
	void OnCountdownFinished();
	
	bool mDidCountdownStart = false;
	FTimerHandle mCountdownTimerHandle;
	uint8 mRemainingCounts;
};
