
#include "BattleRoyale/core/Abilities/AbilityJump.h"
#include "AbilitiesInput.h"
#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"
#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"

UAbilityJump::UAbilityJump()
{
	AbilityInputID = EAbilityInputID::Jump;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;

	AbilityTags.AddTag(TAG_ABILITY_JUMP);
}

void UAbilityJump::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
				TAG_GAMEPLAYCUE_JUMP,
				EGameplayCueEvent::Type::OnActive,
				FGameplayCueParameters());
			UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				ActorInfo->AvatarActor.Get(),
				TAG_GAMEPLAYCUE_JUMP,
				EGameplayCueEvent::Type::WhileActive,
				FGameplayCueParameters());
			
			character->StartJumping();
		}
	}
}

bool UAbilityJump::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
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

	return character != nullptr && character->CanJump();
}

void UAbilityJump::InputReleased(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
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
void UAbilityJump::CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility)
{
	if (ScopeLockCount > 0)
	{
		WaitingToExecute.Add(FPostLockDelegate::CreateUObject(this, &UAbilityJump::CancelAbility, Handle, ActorInfo,
		                                                      ActivationInfo, bReplicateCancelAbility));
		return;
	}

	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);

	const auto character = GetCharacter(ActorInfo);
	if (character != nullptr)
	{
		UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				ActorInfo->AvatarActor.Get(),
				TAG_GAMEPLAYCUE_JUMP,
				EGameplayCueEvent::Type::Removed,
				FGameplayCueParameters());
		character->StopJumping_();
	}
}
