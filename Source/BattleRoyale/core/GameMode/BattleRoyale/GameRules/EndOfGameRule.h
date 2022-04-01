// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/GameRules/IGameRule.h"
#include "EndOfGameRule.generated.h"

class IIGameState;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UEndOfGameRule: public UObject, public IIGameRule
{
	GENERATED_BODY()

public:
	UEndOfGameRule() = default;

	virtual void Initialize(TScriptInterface<IIGameState> gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(TArray<TScriptInterface<IIGameRule>>& rules) const override;

private:
	UPROPERTY()
	TScriptInterface<IIGameState> mGameState;
};
