// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityDrop.h"

#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/IGunComponent.h"

UAbilityDrop::UAbilityDrop()
{
	AbilityInputID = EAbilityInputID::Drop;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_DROP));
}

bool UAbilityDrop::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	if(const auto character = Cast<IICharacter>(ActorInfo->AvatarActor))
	{
		const auto gunComponent = character->GetGunComponent();
		return gunComponent->HasWeaponEquipped();
	}
	return false;
}

void UAbilityDrop::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(const auto character = Cast<IICharacter>(ActorInfo->AvatarActor))
	{
		//const auto gunComponent = character->GetGunComponent();
		character->UnEquipWeaponServer();
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UAbilityDrop::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}
