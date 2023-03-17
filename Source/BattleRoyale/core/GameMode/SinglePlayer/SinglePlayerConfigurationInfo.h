#pragma once
#include "BattleRoyale/core/GameMode/Configuration/GameModeConfigurationInfo.h"
#include "SinglePlayerConfigurationInfo.generated.h"

UCLASS(BlueprintType)
class USinglePlayerConfigurationInfo  : public UGameModeConfigurationInfo
{
	GENERATED_BODY()

public:
	float GetOnceMatchEndedTimeToResetSessionTime() const { return OnceMatchEndedTimeToResetSessionTime; }
	
private:
	UPROPERTY(EditAnywhere, Category=BattleRoyale)
	float OnceMatchEndedTimeToResetSessionTime = 15.0f;
};
