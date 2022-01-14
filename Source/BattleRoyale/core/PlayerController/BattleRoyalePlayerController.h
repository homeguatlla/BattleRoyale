// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPlayerController.h"
#include "GameFramework/PlayerController.h"
#include "BattleRoyalePlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API ABattleRoyalePlayerController : public APlayerController, public IIPlayerController
{

	GENERATED_BODY()

public:
	virtual void EnableInput(bool enable) override;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION(Client, Reliable)
	void ClientEnableInput(bool enable);

};
