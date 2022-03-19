// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "core/Utils/EventDispatcher.h"
#include "Engine/GameInstance.h"
#include "BattleRoyaleGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UBattleRoyaleGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UEventDispatcher* GetEventDispatcher();

private:	

	UPROPERTY(Transient)
	UEventDispatcher* mEventDispatcher;
};
