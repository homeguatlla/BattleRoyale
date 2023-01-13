// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TakeDamageData.generated.h"

/**
 * 
 */
USTRUCT()
struct BATTLEROYALE_API FTakeDamageData
{
	GENERATED_BODY()

	UPROPERTY()
	float health = 0.0f;
	UPROPERTY()
	float damage = 0.0f;
	UPROPERTY()
	ACharacter* playerCauser = nullptr;
};
