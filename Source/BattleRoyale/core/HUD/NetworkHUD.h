// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NetworkHUD.generated.h"

UCLASS()
class BATTLEROYALE_API ANetworkHUD : public AActor
{
	GENERATED_BODY()

public:
	void Initialize(int hudIndex, APlayerController* playerController, TArray<TSubclassOf<UUserWidget>> widgetClasses);

private:
	void BindToDelegate();
	
	UFUNCTION()
	void OnShowConnectivity(int ping);

public:
	UPROPERTY()
	UUserWidget* mHUDWidget;
};
