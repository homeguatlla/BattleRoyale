// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityHurt.h"

#include "AbilitySystemInterface.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"


UAbilityHurt::UAbilityHurt()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//Like a passive ability: will be granted by an gameplay effect, GE_AddHurtAbility
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	//and will be activated when granted(added)
	ActivateAbilityOnGranted = true;
	
	AbilityTags.AddTag(TAG_ABILITY_HURT);
	ActivationOwnedTags.AddTag(TAG_STATE_CAN_BE_HURT);
}

void UAbilityHurt::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const auto character = Cast<IICharacter>(CurrentActorInfo->AvatarActor);
	if(!character)
	{
		return;
	}

	if(const auto hurtComponent = character->GetHurtComponent())
	{
		hurtComponent->RegisterToHealthAttributeDelegate(std::bind(&UAbilityHurt::OnHealthChanged, this, std::placeholders::_1));
	}
}

void UAbilityHurt::OnAvatarSet(const FGameplayAbilityActorInfo * ActorInfo, const FGameplayAbilitySpec & Spec)
{
	Super::OnAvatarSet(ActorInfo, Spec);
	//override method executed when granted.
	//this way if the ability should activate when granted we will try to activate it (passive ability)
	if (ActivateAbilityOnGranted)
	{
		bool ActivatedAbility = ActorInfo->AbilitySystemComponent->TryActivateAbility(Spec.Handle, false);
	}
}

void UAbilityHurt::OnHealthChanged(const FOnAttributeChangeData& data) const
{
	if(!data.GEModData)
	{
		return;
	}

	const auto victim = Cast<IICharacter>(CurrentActorInfo->AvatarActor);
	const auto attributeSetHealth = victim->GetAbilitySystemComponent()->GetAbilitySystemComponent()->GetSet<UAttributeSetHealth>();
	if(victim)
	{
		const auto currentHealth = attributeSetHealth->GetHealth();
		victim->NotifyRefreshHealth(currentHealth);
	}
	
	//const auto instigator = data.GEModData->EffectSpec.GetEffectContext().GetInstigator();
}
