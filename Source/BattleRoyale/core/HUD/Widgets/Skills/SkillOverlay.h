// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/Overlay.h"
#include "SkillOverlay.generated.h"

class USkillWidget;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API USkillOverlay : public UOverlay
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void SetItem(float cooldownTime, const TSubclassOf<USkillWidget>& widget);
	UFUNCTION(BlueprintCallable)
	void StartCooldown();

private:
	
	
	UPROPERTY()
	TSubclassOf<USkillWidget> CurrentWidgetClass;
	UPROPERTY()
	USkillWidget* mWidget {nullptr};
	
	FGameplayTag mSkillTag;
	float mCooldownTime = 0.0f;
};
