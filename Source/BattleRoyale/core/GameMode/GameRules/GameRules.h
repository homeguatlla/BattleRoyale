// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGameRule.h"
#include "GameRules.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UGameRules: public UObject
{
	GENERATED_BODY()
	
public:
	UGameRules();

	void AddRule(TScriptInterface<IIGameRule> rule);
	void Execute();

	//Test purposes
	int GetNumRules() const { return mRules.Num(); }
private:
	UPROPERTY()
	TArray<TScriptInterface<IIGameRule>> mRules;
};
