// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "AbilityHurt.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilityHurt : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityHurt();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//This method is overriden in order to implement ability activation when granted (like a passive ability)
	//https://github.com/tranek/GASDocumentation#concepts-ga-activating-passive
	virtual void OnAvatarSet(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

private:
	void OnHealthChanged(const FOnAttributeChangeData& data) const;
};
