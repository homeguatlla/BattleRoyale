// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/GameRules/IGameRule.h"
#include "CheckThereIsOnlyOneTeamAlive.generated.h"

class IIGameState;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UCheckThereIsOnlyOneTeamAlive: public UObject, public IIGameRule
{
	GENERATED_BODY()

public:
	UCheckThereIsOnlyOneTeamAlive() = default;

	virtual void Initialize(TScriptInterface<IIGameState> gameState) override;
	virtual bool Evaluate() const override;
	virtual void Execute() const override;

private:
	UPROPERTY()
	TScriptInterface<IIGameState> mGameState;
};
