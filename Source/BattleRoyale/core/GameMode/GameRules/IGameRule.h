// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameRule.generated.h"

class IIGameState;
// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIGameRule : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIGameRule
{
	GENERATED_BODY()

public:

	virtual void Initialize(TScriptInterface<IIGameState> gameState) = 0;
	virtual bool Evaluate() = 0;
	//return true if the list have been modified
	virtual bool Execute(TArray<TScriptInterface<IIGameRule>>& rules) const = 0;
};
