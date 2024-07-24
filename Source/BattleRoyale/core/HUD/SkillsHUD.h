// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SkillsHUD.generated.h"

UCLASS()
class BATTLEROYALE_API ASkillsHUD : public AActor
{
	GENERATED_BODY()

public:
	void Initialize(int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> widgetClasses);

private:
	void BindToDelegate();
	
	UFUNCTION()
	void OnRefreshSkillsCooldowns();

	UPROPERTY()
	UUserWidget* mHUDWidget;
};
