// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyaleConfigurationInfo.h"
#include "BattleRoyale/core/GameMode/GameModeCommon.h"
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

	float GetOnceMatchEndedTimeToResetSessionTime() const;
	uint8 GetNumPlayersPerTeam() const;
	float GetCountdownTimeToStartGame() const;
	uint8 GetNumTeamsToStartCountDown() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	TSubclassOf<APawn> CharacterClass;

private:
	
	void InitializeTeamSelectionStrategy();
	void ApplyTeamSelectionStrategy(const AController* controller) const;
	UBattleRoyaleConfigurationInfo* GetConfigurationInfo() const;
	
	UPROPERTY()
	TScriptInterface<ITeamSelectionStrategy> mTeamSelectionStrategy;
};



