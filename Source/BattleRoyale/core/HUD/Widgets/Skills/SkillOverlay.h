// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Overlay.h"
#include "SkillOverlay.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API USkillOverlay : public UOverlay
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	void SetItem(int cooldownTime, const TSubclassOf<UUserWidget>& widget);
	
private:
	UPROPERTY()
	TSubclassOf<UUserWidget> CurrentWidgetClass;

	float mCooldownTime = 0.0;
};
