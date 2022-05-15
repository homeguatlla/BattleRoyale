// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IGameMode.generated.h"

class IIGameState;

UINTERFACE(MinimalAPI)
class UIGameMode : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IIGameMode
{
	GENERATED_BODY()

public:
	virtual void DoGenereicPlayerInitialization(AController* controller) {}
	virtual void DoInitGame() {}
	
	virtual IIGameState* GetGameState() const = 0;

	virtual void DestroyGameSession() const = 0;
	virtual bool HasCurrentGameSession() const = 0;
	
	virtual void OnNewKill(const APlayerController* killerController, const APlayerController* victimController) = 0;
	virtual bool CanPlayerCauseDamageTo(const APlayerController* killerController, const APlayerController* victimController) = 0;
};
