// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "AbilityPickupIndicator.generated.h"

struct FTargetDataPickupObject;
/**
 * 
 */
UCLASS(Blueprintable)
class BATTLEROYALE_API UAbilityPickupIndicator : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityPickupIndicator();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnShowPickupIndicator(const FVector& Location);
	UFUNCTION(BlueprintImplementableEvent)
	void OnHidePickupIndicator();
	
private:
	bool FillWithGameplayEventData(const FGameplayEventData* TriggerEventData, FTargetDataPickupObject& pickupIndicatorData) const;
};
