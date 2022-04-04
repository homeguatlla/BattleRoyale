// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ITeamSelectionStrategy.h"
#include "SimpleTeamSelectionStrategy.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API USimpleTeamSelectionStrategy: public UObject, public ITeamSelectionStrategy
{
	GENERATED_BODY()

public:
	USimpleTeamSelectionStrategy() = default;

	virtual void Initialize(int numPlayersPerTeam) override;
	virtual int GetNextTeamId() override;

private:
	int mNumPlayersPerTeam = 1;
	int mLastSequenceIndexGenerated;
};
