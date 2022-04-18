// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/GameRules/TimerRule.h"
#include "ShowStatsScreenRule.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UShowStatsScreenRule: public UTimerRule
{
	GENERATED_BODY()

public:
	UShowStatsScreenRule() = default;

	virtual void Initialize(TScriptInterface<IIGameState> gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(TArray<TScriptInterface<IIGameRule>>& rules) const override;

private:
	UPROPERTY()
	TScriptInterface<IIGameState> mGameState;
};
