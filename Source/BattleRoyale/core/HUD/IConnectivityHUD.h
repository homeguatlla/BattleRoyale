// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IConnectivityHUD.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable)
class UConnectivityHUD : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IConnectivityHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Connectivity")
	void OnShowConnectivity(int ping);
};
