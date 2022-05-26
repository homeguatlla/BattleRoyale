// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CooldownGameplayEffect.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UCooldownGameplayEffect : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UCooldownGameplayEffect();
	void SetDuration(float duration);
};

inline UCooldownGameplayEffect::UCooldownGameplayEffect()
{
	DurationPolicy = EGameplayEffectDurationType::HasDuration;	
}

inline void UCooldownGameplayEffect::SetDuration(float duration)
{
	DurationMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(duration));
}
