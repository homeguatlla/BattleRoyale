
#include "BattleRoyale/core/Abilities/AbilitySprint.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayTagsList.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"

UAbilitySprint::UAbilitySprint()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_SPRINTING));

	//Prevents activate sprint ability if aiming.
	//But if we are already sprinting, the ability is not cancelled.
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
	
	//To enable sprint ability from a trigger
	FAbilityTriggerData triggerData;
	triggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_SPRINT);
	triggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerData);
}

void UAbilitySprint::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                     const FGameplayAbilityActorInfo* ActorInfo,
                                     const FGameplayAbilityActivationInfo ActivationInfo,
                                     const FGameplayEventData* TriggerEventData)
{
	if (HasAuthorityOrPredictionKey(ActorInfo, &ActivationInfo))
	{
		if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
		{
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
			return;
		}

		const auto character = GetCharacter(ActorInfo);
		const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
		if (character != nullptr && abilitySystemComponent != nullptr)
		{
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				ActorInfo->AvatarActor.Get(),
				FGameplayTag::RequestGameplayTag(TAG_GAMEPLAYCUE_SPRINT),
				EGameplayCueEvent::Type::OnActive,
				FGameplayCueParameters());
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				ActorInfo->AvatarActor.Get(),
				FGameplayTag::RequestGameplayTag(TAG_GAMEPLAYCUE_SPRINT),
				EGameplayCueEvent::Type::WhileActive,
				FGameplayCueParameters());

			if(ActorInfo->IsNetAuthority() && SprintSpeedEffect)
			{
				abilitySystemComponent->ApplyGameplayEffectToSelf(SprintSpeedEffect);
			}
			
			abilitySystemComponent->RegisterGameplayEvent(
				FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_WALK)),
				FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnWalk));
			character->StartSprinting();
				
			return;
		}
	}
	
	EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
}

bool UAbilitySprint::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo,
                                        const FGameplayTagContainer* SourceTags,
                                        const FGameplayTagContainer* TargetTags,
                                        OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}
	const auto character = GetCharacter(ActorInfo);

	return character != nullptr && character->CanSprint();
}
/*
void UAbilitySprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}*/

// Epic's comment
/**
 *	Canceling an non instanced ability is tricky. Right now this works for Jump since there is nothing that can go wrong by calling
 *	StopJumping() if you aren't already jumping. If we had a montage playing non instanced ability, it would need to make sure the
 *	Montage that *it* played was still playing, and if so, to cancel it. If this is something we need to support, we may need some
 *	light weight data structure to represent 'non intanced abilities in action' with a way to cancel/end them.
 */
void UAbilitySprint::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UAbilitySprint::CancelAbility, Handle, ActorInfo,
		                                                      ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	const auto character = GetCharacter(ActorInfo);
	const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
	if (character != nullptr && abilitySystemComponent != nullptr)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				ActorInfo->AvatarActor.Get(),
				FGameplayTag::RequestGameplayTag(TAG_GAMEPLAYCUE_SPRINT),
				EGameplayCueEvent::Type::Removed,
				FGameplayCueParameters());

		if(ActorInfo->IsNetAuthority() && WalkSpeedEffect)
		{
			abilitySystemComponent->ApplyGameplayEffectToSelf(WalkSpeedEffect);
		}
		character->StopSprinting();
	}
}

void UAbilitySprint::OnWalk(FGameplayTag gameplayTag, const FGameplayEventData* playload)
{
	K2_CancelAbility();
}
