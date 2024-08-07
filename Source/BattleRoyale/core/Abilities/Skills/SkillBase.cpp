// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillBase.h"

#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/EventDispatcher.h"

USkillBase::USkillBase()
{
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	//CooldownGameplayEffectClass = UCooldownGameplayEffect::StaticClass();
}

void USkillBase::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

    if(IsLocallyControlled())
    {
	    NotifyOnStartSkillCooldown(ActorInfo->AvatarActor);
    }
}

void USkillBase::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	if(IsLocallyControlled())
	{
		NotifyOnStopSkillCooldown(ActorInfo->AvatarActor);
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void USkillBase::NotifyOnStartSkillCooldown(const TWeakObjectPtr<AActor> avatarActor) const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(avatarActor->GetGameInstance());
	gameInstance->GetEventDispatcher()->OnStartSkillCooldown.Broadcast(AbilityTags.First());
}

void USkillBase::NotifyOnStopSkillCooldown(const TWeakObjectPtr<AActor> avatarActor) const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(avatarActor->GetGameInstance());
	gameInstance->GetEventDispatcher()->OnStopSkillCooldown.Broadcast(AbilityTags.First());
}

float USkillBase::GetCooldownDuration() const
{
	const auto cooldownEffect = GetCooldownGameplayEffect();
	if(!cooldownEffect)
	{
		return 0.0f;
	}

	float duration;
	cooldownEffect->DurationMagnitude.GetStaticMagnitudeIfPossible(1, duration);

	return duration;
}

