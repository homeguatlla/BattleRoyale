// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityPickupIndicator.h"

#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupIndicator.h"
#include "Components/WidgetComponent.h"

UAbilityPickupIndicator::UAbilityPickupIndicator()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP_INDICATOR));
	
	//With this trigger we activate the ability if it receives an gameplay event with the event_pickup_indicator tag
	FAbilityTriggerData triggerData;
	triggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_PICKUP_INDICATOR);
	triggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerData);
}

bool UAbilityPickupIndicator::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UAbilityPickupIndicator::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FTargetDataPickupIndicator pickupIndicatorData;
	
	if (!FillWithGameplayEventData(TriggerEventData, pickupIndicatorData))
	{
		K2_EndAbility();
		return;
	}

	if(IsLocallyControlled())
	{
		OnShowPickupIndicator(pickupIndicatorData.Location);
	}
}

void UAbilityPickupIndicator::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsLocallyControlled())
	{
		OnHidePickupIndicator();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UAbilityPickupIndicator::FillWithGameplayEventData(const FGameplayEventData* TriggerEventData, FTargetDataPickupIndicator& pickupIndicatorData) const
{
	if(TriggerEventData && TriggerEventData->TargetData.Num() > 0)
	{
		const FGameplayAbilityTargetData* targetData = TriggerEventData->TargetData.Data[0].Get();
		utils::UtilsLibrary::RetrieveGameplayEventTargetData<FTargetDataPickupIndicator>(TriggerEventData, pickupIndicatorData);
		return true;
	}
	return false;
}
