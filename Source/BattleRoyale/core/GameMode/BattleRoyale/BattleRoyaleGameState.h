// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "GameFramework/GameState.h"
#include "BattleRoyaleGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshCountdown, uint8, counter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishCountdown);

UCLASS()
class BATTLEROYALE_API ABattleRoyaleGameState : public AGameState, public IIGameState
{
	GENERATED_BODY()
public:
	ABattleRoyaleGameState();
	
	virtual bool DidCountdownStart() const override{ return mDidCountdownStart; }
	virtual void StartCountdownServer(uint8 duration) override;
	virtual bool DidCountdownFinish() const override { return mRemainingCounts <= 0; }
	
	UPROPERTY(BlueprintAssignable, Category=BattleRoyaleGameState)
	FOnRefreshCountdown OnRefreshCountDownDelegate;

	UPROPERTY(BlueprintAssignable, Category=BattleRoyaleGameState)
	FOnFinishCountdown OnFinishCountDownDelegate;
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void OnCountdownFinishedServer();
	
	UFUNCTION()
	void OnRep_RemainingCount() const;
	
	bool mDidCountdownStart = false;
	FTimerHandle mCountdownTimerHandle;

	UPROPERTY(ReplicatedUsing=OnRep_RemainingCount)
	uint8 mRemainingCounts;
};
