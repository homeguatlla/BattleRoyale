#pragma once
#include "IPlayerHUD.generated.h"

UINTERFACE(Blueprintable)
class UPlayerHUD : public UInterface
{
	GENERATED_BODY()
};

class IPlayerHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Player HUD")
	void OnPlayerDead();
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Player HUD")
	void OnRefreshNumKills(int numKills);
};