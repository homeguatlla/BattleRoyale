// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/GameRules/IGameRule.h"
#include "TimerRule.generated.h"

class IIGameState;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UTimerRule: public UObject, public IIGameRule
{
	GENERATED_BODY()

public:
	UTimerRule() = default;
	virtual void BeginDestroy() override;
	
	virtual void Initialize(TScriptInterface<IIGameState> gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(TArray<TScriptInterface<IIGameRule>>& rules) const override;

	void SetDuration(float duration) { mDuration = duration;}
	void Start();
	
private:
	void OnTimerFinished();
	
	FTimerHandle mTimerHandle;
	bool mIsTimerOver;
	float mDuration;
};

