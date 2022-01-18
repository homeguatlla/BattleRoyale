// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "GameFramework/PlayerState.h"
#include "BattleRoyalePlayerState.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API ABattleRoyalePlayerState : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()

public:
	
	ABattleRoyalePlayerState();
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return mAbilitySystemComponent; }
private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AbilitySystemComponent, meta = (AllowPrivateAccess = "true"))
	class UAbilitySystemComponent* mAbilitySystemComponent;
};
