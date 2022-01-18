// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameState.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIGameState : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIGameState
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGameState")
	virtual bool DidCountdownStart() const = 0;
	
	UFUNCTION(BlueprintCallable, BlueprintCosmetic, Category = "IGameState")
	virtual bool DidCountdownFinish() const = 0;

	virtual void StartCountdownServer(uint8 duration) = 0;
};
