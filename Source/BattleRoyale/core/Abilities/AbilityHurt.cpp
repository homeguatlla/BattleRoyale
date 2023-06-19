// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityHurt.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectExtension.h"
#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameMode.h"


UAbilityHurt::UAbilityHurt()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//Like a passive ability: will be granted by an gameplay effect, GE_AddHurtAbility
	//Esta habilidad si es LocalOnly, no funcionará cuando dispare el servidor!!? no se modifica la vida. Averiguar porque.
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	//and will be activated when granted(added)
	ActivateAbilityOnGranted = true;
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_HURT));
}

void UAbilityHurt::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	const auto character = Cast<IICharacter>(CurrentActorInfo->AvatarActor);
	if(!character || !ActorInfo->IsNetAuthority())
	{
		return;
	}

	//CurrentActorInfo->AbilitySystemComponent->SetLooseGameplayTagCount(FGameplayTag::RequestGameplayTag(TAG_STATE_CAN_BE_HURT), 1);
	//Only make sense subscribe to the attribute change if server
	if(const auto hurtComponent = character->GetHurtComponent())
	{
		hurtComponent->RegisterToHealthAttributeDelegate(std::bind(&UAbilityHurt::OnHealthChanged, this, std::placeholders::_1));
	}
	//ActorInfo->AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_CAN_BE_HURT));
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
	const auto currentHealth = attributeSetHealth->GetHealth();
		
	if(victim)
	{
		victim->NotifyRefreshHealth(currentHealth);
	}

	const auto causerPlayerState = Cast<APlayerStateBase>(data.GEModData->EffectSpec.GetEffectContext().GetInstigator());
	//const auto causer = causerPlayerState->GetCharacter();

	//TODO aquí lo suyo sería pasar el IICharacter pero choca con que el propio IICharacter tenga una definición de él mismo
	victim->NotifyTakeDamage(data.OldValue-data.NewValue, causerPlayerState->GetPawn(), currentHealth);
}