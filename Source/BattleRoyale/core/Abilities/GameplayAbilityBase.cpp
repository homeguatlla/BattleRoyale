

#include "BattleRoyale/core/Abilities/GameplayAbilityBase.h"
#include "AbilitySystemComponent.h"
#include "GameplayTagContainer.h"
#include "BattleRoyale/core/Character/ICharacter.h"

UGameplayAbilityBase::UGameplayAbilityBase()
{
	// Default to Instance Per Actor
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Default tags that block this ability from activating
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
	//ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Debuff.Stun")));
}

void UGameplayAbilityBase::OnAvatarSet(const FGameplayAbilityActorInfo* actorInfo, const FGameplayAbilitySpec& spec)
{
	Super::OnAvatarSet(actorInfo, spec);

	if (ActivateAbilityOnGranted)
	{
		bool ActivatedAbility = actorInfo->AbilitySystemComponent->TryActivateAbility(spec.Handle, false);
	}
}

IICharacter* UGameplayAbilityBase::GetCharacter(const FGameplayAbilityActorInfo* actorInfo) const
{
	const auto character = actorInfo->AvatarActor.Get();
	if(character != nullptr && character->Implements<UICharacter>())
	{
		return Cast<IICharacter>(character);
	}
	return nullptr;
}
