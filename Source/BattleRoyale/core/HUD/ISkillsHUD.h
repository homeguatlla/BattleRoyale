// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISkillsHUD.generated.h"


UINTERFACE()
class UISkillsHUD : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API IISkillsHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Inventory HUD")
	void OnRefreshSkillsCooldowns();
};
