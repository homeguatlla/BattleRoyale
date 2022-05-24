#pragma once
#include "BattleRoyale/core/GameMode/Configuration/GameModeConfigurationInfo.h"
#include "BattleRoyaleConfigurationInfo.generated.h"

UCLASS(BlueprintType)
class UBattleRoyaleConfigurationInfo  : public UGameModeConfigurationInfo
{
	GENERATED_BODY()

public:
	float GetOnceMatchEndedTimeToResetSessionTime() const { return OnceMatchEndedTimeToResetSessionTime; }
	uint8 GetNumPlayersPerTeam() const { return NumPlayersPerTeam; }
	float GetCountdownTimeToStartGame() const { return CountdownTimeToStartGame; }
	
private:
	UPROPERTY(EditAnywhere, Category=BattleRoyale)
	float OnceMatchEndedTimeToResetSessionTime = 15.0f;

	UPROPERTY(EditAnywhere, Category=BattleRoyale)
	uint8 NumPlayersPerTeam = 1;
	
	UPROPERTY(EditAnywhere, Category=BattleRoyale)
	float CountdownTimeToStartGame = 15.f;
};
