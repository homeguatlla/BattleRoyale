// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityHurt.h"

#include "GameplayEffectExtension.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"


UAbilityHurt::UAbilityHurt()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//Like a passive ability: will be granted by an gameplay effect, GE_AddHurtAbility
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	//and will be activated when granted(added)
	ActivateAbilityOnGranted = true;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(FName("Ability.Hurt")));
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

	if(auto hurtComponent = character->GetHurtComponent())
	{
		hurtComponent->RegisterToHealthAttributeDelegate(std::bind(&UAbilityHurt::OnHealthChanged, this, std::placeholders::_1));
	}
}

bool UAbilityHurt::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);
}

void UAbilityHurt::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
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
	//const auto abilitySystemComponent = GetPlayerStateInterface()->GetAbilitySystemComponent();
		
	//if(abilitySystemComponent)
	{
		if(!data.GEModData)
		{
			return;
		}
		
		const auto instigator = data.GEModData->EffectSpec.GetEffectContext().GetInstigator();
		if(instigator)
		{
			
		}
		
		//TODO aquí deberíamos actualizar el HUD
		//este método podría ser llamado por otras heridas.
		//const auto attributes = abilitySystemComponent->GetSet<UAttributeSetHealth>();
		//UE_LOG(LogCharacter, Error, TEXT("[ACharacterBase::OnHealthChanged] Health current value %f"), attributes->GetHealth());	
	}
}
