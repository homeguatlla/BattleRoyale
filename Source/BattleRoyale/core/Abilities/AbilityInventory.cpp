// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityInventory.h"

#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"

UAbilityInventory::UAbilityInventory()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_INVENTORY));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_AIM));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));

	FAbilityTriggerData triggerDataToAdd;
	triggerDataToAdd.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_SHOW_INVENTORY);
	triggerDataToAdd.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerDataToAdd);
}

void UAbilityInventory::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(const auto inventoryComponent = GetInventoryComponent(ActorInfo))
	{
		const auto character = GetCharacter(ActorInfo);
		if (character != nullptr)
		{
			const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
			check(abilitySystemComponent);
			m_HideInventoryHandle = abilitySystemComponent->RegisterGameplayEvent(
					FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_HIDE_INVENTORY)),
					FGameplayEventTagMulticastDelegate::FDelegate::CreateUObject(this, &ThisClass::OnHideInventory));
		}
		inventoryComponent->ShowHideInventory();
	}
}

TScriptInterface<IIInventoryComponent> UAbilityInventory::GetInventoryComponent(const FGameplayAbilityActorInfo* ActorInfo)
{
	if (const auto character = GetCharacter(ActorInfo))
	{
		return character->GetInventoryComponent();
	}
	
	return nullptr;
}

void UAbilityInventory::OnHideInventory(FGameplayTag gameplayTag, const FGameplayEventData* payload)
{
	if(const auto inventoryComponent = GetInventoryComponent(GetCurrentActorInfo()))
	{
		inventoryComponent->ShowHideInventory();
		if(!inventoryComponent->IsInventoryVisible())
		{
			const auto character = GetCharacter(GetCurrentActorInfo());
			if (character != nullptr)
			{
				const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
				check(abilitySystemComponent);
				abilitySystemComponent->UnRegisterGameplayEvent(
					FGameplayTagContainer(FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_HIDE_INVENTORY)),
					m_HideInventoryHandle);
			}
			K2_EndAbility();
		}
	}
}
