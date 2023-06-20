// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityPickUp.h"

#include "BlueprintGameplayTagLibrary.h"
#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"

UAbilityPickUp::UAbilityPickUp()
{
	AbilityInputID = EAbilityInputID::Equip;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_EQUIP));
}

bool UAbilityPickUp::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UAbilityPickUp::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString("AbilityEquip ON"));

	if(const auto character = Cast<IICharacter>(ActorInfo->AvatarActor))
	{
		if(const auto pickupObject = character->GetPickupObject())
		{
			if(character->PickupObjectServer(pickupObject))
			{
				CancelPickupIndicatorAbility(character);
			}
		}
	}
	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UAbilityPickUp::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString("AbilityEquip OFF"));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbilityPickUp::CancelPickupIndicatorAbility(IICharacter* const character) const
{
	FGameplayTagContainer cancelTags;
	UBlueprintGameplayTagLibrary::AddGameplayTag(cancelTags, FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP_INDICATOR));
	character->GetAbilitySystemComponentBase()->CancelAbilitiesWithTags(cancelTags);
}