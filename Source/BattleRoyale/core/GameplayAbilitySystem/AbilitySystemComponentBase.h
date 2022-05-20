// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "IAbilitySystemInterfaceBase.h"
#include "UObject/Object.h"
#include "AbilitySystemComponentBase.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilitySystemComponentBase : public UAbilitySystemComponent, public IIAbilitySystemInterfaceBase
{
	GENERATED_BODY()

public:

	virtual void SetSimulatedMontage(UAnimMontage* montage) override;
	/*virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f) override;
	virtual float PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;*/
};
