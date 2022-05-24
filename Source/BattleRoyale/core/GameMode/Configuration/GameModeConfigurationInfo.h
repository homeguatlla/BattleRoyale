#pragma once
#include "GameModeConfigurationInfo.generated.h"

class UMultiplayerConfigurationInfo;

UCLASS(BlueprintType)
class UGameModeConfigurationInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UMultiplayerConfigurationInfo* GetMultiplayerConfigurationInfo() const { return MultiplayerConfigurationInfo; }
	float GetGameRulesUpdateIntervalTime() const { return GameRulesUpdateIntervalTime; }
	float GetTimeBetweenEndOfGameAndStatsScreen() const { return TimeBetweenEndOfGameAndStatsScreen; }
	
private:

	UPROPERTY(EditAnywhere, Category="Multiplayer GameSession")
	UMultiplayerConfigurationInfo* MultiplayerConfigurationInfo;

	UPROPERTY(EditAnywhere, Category="GameRules")
	float GameRulesUpdateIntervalTime = 1.0f;

	UPROPERTY(EditAnywhere, Category="GameMode")
	float TimeBetweenEndOfGameAndStatsScreen = 5.0f;
};
