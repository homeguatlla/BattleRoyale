// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStatsData.generated.h"


USTRUCT(BlueprintType)
struct FPlayerStatsData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int mNumKills;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FString mPlayerName;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	int mTeamId;
};