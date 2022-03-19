// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "IPlayerState.generated.h"

// This class does not need to be modified.
UINTERFACE(Blueprintable, meta = (CannotImplementInterfaceInBlueprint))
class UIPlayerState : public UAbilitySystemInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIPlayerState: public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentInterface() const = 0;
};
