// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ISkillsHUD.generated.h"


UINTERFACE()
class USkillsHUD : public UInterface
{
	GENERATED_BODY()
};

class BATTLEROYALE_API ISkillsHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Skills HUD")
	void OnInitializeSkills(const TArray<FSkillData>& skills);
	
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Skills HUD")
	void OnStartSkillCooldown(const FGameplayTag& skillTag);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Skills HUD")
	void OnStopSkillCooldown(const FGameplayTag& skillTag);
};
