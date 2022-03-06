#pragma once
#include "IHealthHUD.generated.h"

UINTERFACE(Blueprintable)
class UHealthHUD : public UInterface
{
	GENERATED_BODY()
};

class IHealthHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Health")
	void OnRefreshHealth(float health);
};