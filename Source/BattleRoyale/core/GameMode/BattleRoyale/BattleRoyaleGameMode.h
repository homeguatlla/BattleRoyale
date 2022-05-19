// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/GameModeCommon.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameMode.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/Utils/TeamSelectionStrategies/ITeamSelectionStrategy.h"
#include "BattleRoyaleGameMode.generated.h"

UCLASS(minimalapi)
class ABattleRoyaleGameMode : public AGameModeCommon
{
	GENERATED_BODY()

public:
	ABattleRoyaleGameMode();

	virtual void DoInitGame() override;
	virtual void DoGenereicPlayerInitialization(AController* controller) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	TSubclassOf<APawn> CharacterClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	uint8 NumPlayersPerTeam = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	float CountdownTimeToStartGame = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	float GameRulesUpdateIntervalTime = 1.0f;

private:
	
	void InitializeTeamSelectionStrategy();
	void ApplyTeamSelectionStrategy(const AController* controller) const;

	UPROPERTY()
	TScriptInterface<ITeamSelectionStrategy> mTeamSelectionStrategy;
};



