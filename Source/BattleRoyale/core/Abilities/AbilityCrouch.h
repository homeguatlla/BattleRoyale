// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "AbilityCrouch.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilityCrouch : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityCrouch();
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                     FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                        const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
	                        FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void InputReleased(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                   FGameplayAbilityActivationInfo ActivationInfo) override;
	virtual void CancelAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                   FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
};
