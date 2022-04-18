// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ITeamSelectionStrategy.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UTeamSelectionStrategy : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API ITeamSelectionStrategy
{
	GENERATED_BODY()

public:
	virtual void Initialize(int numPlayersPerTeam) = 0;
	virtual int GetNextTeamId() = 0;
};
