#pragma once
#include "BattleRoyale/core/Utils/Stats/PlayerStatsData.h"
#include "IStatsMenuHUD.generated.h"

UINTERFACE(Blueprintable)
class UStatsMenuHUD : public UInterface
{
	GENERATED_BODY()
};

class IStatsMenuHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Menu Stats HUD")
	void OnShowStatsScreen(const FPlayerStatsData& playerStatsData);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Menu Stats HUD")
	void OnHideStatsScreen();
};