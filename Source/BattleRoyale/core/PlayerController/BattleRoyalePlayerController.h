// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BattleRoyalePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API ABattleRoyalePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;
};
