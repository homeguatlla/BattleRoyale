#pragma once
#include "ICounterDownMenuHUD.generated.h"

UINTERFACE(Blueprintable)
class UCounterDownMenuHUD : public UInterface
{
	GENERATED_BODY()
};

class ICounterDownMenuHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Menu Countdown HUD")
	void OnRefreshCountDown(uint8 counter);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Menu Countdown HUD")
	void OnFinishCountDown();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Menu Countdown HUD")
	void OnSetVisibilityCountDown(bool visible);
};