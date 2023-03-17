// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/GameModeCommon.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "SinglePlayerGameMode.generated.h"

class USinglePlayerConfigurationInfo;
UCLASS(minimalapi)
class ASinglePlayerGameMode : public AGameModeCommon
{
	GENERATED_BODY()

public:
	ASinglePlayerGameMode();

	virtual void DoInitGame() override;
	virtual void DoGenereicPlayerInitialization(AController* controller) override;

	float GetOnceMatchEndedTimeToResetSessionTime() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=SinglePlayer)
	TSubclassOf<APawn> CharacterClass;

private:
	
	USinglePlayerConfigurationInfo* GetConfigurationInfo() const;
};



