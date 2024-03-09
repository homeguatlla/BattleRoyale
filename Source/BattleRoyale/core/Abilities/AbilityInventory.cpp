// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityInventory.h"

#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"

UAbilityInventory::UAbilityInventory()
{
	AbilityInputID = EAbilityInputID::Inventory;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_INVENTORY));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_AIM));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));
}

void UAbilityInventory::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                        const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                        const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if(const auto inventoryComponent = GetInventoryComponent(ActorInfo))
	{
		inventoryComponent->ShowHideInventory();	
	}
}

void UAbilityInventory::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);

	if(const auto inventoryComponent = GetInventoryComponent(ActorInfo))
	{
		inventoryComponent->ShowHideInventory();
		if(!inventoryComponent->IsInventoryVisible())
		{
			GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, "input pressed");
			EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		}
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