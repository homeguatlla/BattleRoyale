#pragma once
#include "IGameHUD.generated.h"

UINTERFACE(Blueprintable)
class UGameHUD : public UInterface
{
	GENERATED_BODY()
};

class IGameHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Game HUD")
	void OnGameStarted();
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Game HUD")
	void OnGameOver();
};