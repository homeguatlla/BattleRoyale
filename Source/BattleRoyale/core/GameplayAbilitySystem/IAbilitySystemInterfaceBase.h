// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "UObject/Interface.h"
#include "IAbilitySystemInterfaceBase.generated.h"

class UAbilitySystemComponentBase;

// This class does not need to be modified.
UINTERFACE()
class UIAbilitySystemInterfaceBase : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIAbilitySystemInterfaceBase
{
	GENERATED_BODY()

public:
	virtual void SetSimulatedMontage(UAnimMontage* montage) = 0;
};
