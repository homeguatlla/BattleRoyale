
#include "BattleRoyale/core/Abilities/AbilitySprint.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "AbilitiesInput.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayTagsList.h"

UAbilitySprint::UAbilitySprint()
{
	AbilityInputID = EAbilityInputID::Sprint;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SPRINT));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT));	

	//Prevents activate sprint ability if aiming.
	//But if we are already sprinting, the ability is not cancelled.
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_AIMING));
	//To enable sprint ability from a trigger
	/*FAbilityTriggerData triggerData;
	triggerData.TriggerTag = FGameplayTag::RequestGameplayTag(FName("Trigger.Sprint"));
	triggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerData);*/
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
		}

		const auto character = GetCharacter(ActorInfo);
		if (character != nullptr)
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
			
			character->StartSprinting();
		}
	}
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

void UAbilitySprint::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo != NULL && ActorInfo->AvatarActor != NULL)
	{
		CancelAbility(Handle, ActorInfo, ActivationInfo, true);
	}
}

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
	if (character != nullptr)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				ActorInfo->AvatarActor.Get(),
				FGameplayTag::RequestGameplayTag(TAG_GAMEPLAYCUE_SPRINT),
				EGameplayCueEvent::Type::Removed,
				FGameplayCueParameters());
		character->StopSprinting();
	}
}
