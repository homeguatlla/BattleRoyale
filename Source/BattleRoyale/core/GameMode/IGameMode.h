// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameMode.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIGameMode : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIGameMode
{
	GENERATED_BODY()

public:
	virtual void OnNewKill(const APlayerController* killerController, const APlayerController* victimController) = 0;
	virtual bool CanPlayerCauseDamageTo(const APlayerController* killerController, const APlayerController* victimController) = 0;
};
