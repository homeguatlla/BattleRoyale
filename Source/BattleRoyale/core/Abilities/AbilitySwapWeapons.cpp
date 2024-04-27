// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySwapWeapons.h"

#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/IGunComponent.h"

UAbilitySwapWeapons::UAbilitySwapWeapons()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;

	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));
	CancelAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
	
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));

	FAbilityTriggerData triggerDataToAdd;
	triggerDataToAdd.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_SWAP_WEAPONS);
	triggerDataToAdd.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerDataToAdd);
}

bool UAbilitySwapWeapons::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if(!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const auto character = GetCharacter(ActorInfo);
	if(!character || !character->IsAlive())
	{
		return false;
	}
	
	const auto combatComponent = character->GetGunComponent();
	const auto inventoryComponent = character->GetInventoryComponent();
	if(!combatComponent || !inventoryComponent)
	{
		return false;
	}
	
	return combatComponent->CanSwapWeapons(inventoryComponent);
}

void UAbilitySwapWeapons::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const auto character = GetCharacter(ActorInfo);
	if(!character || !character->IsAlive())
	{
		return;
	}

	const auto combatComponent = character->GetGunComponent();
	const auto inventoryComponent = character->GetInventoryComponent();
	if(!combatComponent || !inventoryComponent)
	{
		return;
	}

	combatComponent->SwapWeaponsServer(inventoryComponent);

	K2_EndAbility();
}
