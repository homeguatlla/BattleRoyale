// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityEquip.h"

#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"

UAbilityEquip::UAbilityEquip()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_EQUIP));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_EQUIPPING));

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_HEAL));

	FAbilityTriggerData triggerDataToAdd;
	triggerDataToAdd.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_EQUIP_ITEM_TO_HEAL);
	triggerDataToAdd.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerDataToAdd);
}

void UAbilityEquip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                    const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!TriggerEventData)
	{
		K2_EndAbility();
		return;
	}

	const auto character = GetCharacter(ActorInfo);
	if (!character)
	{
		K2_EndAbility();
		return;
	}
	
	TSubclassOf<UInventoryItemStaticData> itemStaticDataToEquip = nullptr;
	
	if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_EQUIP_ITEM_TO_HEAL))
	{
		if (!CanIHeal(character))
		{
			K2_EndAbility();
			return;
		}
		
		itemStaticDataToEquip = InventoryItemHealStaticData;
	}
	/*if (TriggerEventData->EventTag == FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_EQUIP_ITEM_TO_HEAL))
	{
		// 0) find weapon to equip if it has 
		
	}*/
	
	SubscribeToEventMontageItemBack();
	CreateTaskPlayMontage(ActorInfo, UnEquipAnimation);
}

void UAbilityEquip::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbilityEquip::OnMontageCancelled()
{
}

void UAbilityEquip::OnMontageCompleted()
{
	
}

void UAbilityEquip::OnEventMontageItemBackReceived(FGameplayEventData payload)
{
	const auto character = GetCharacter(GetAvatarActorFromActorInfo());
	if (!character)
	{
		K2_EndAbility();
		return;
	}
	
	const auto inventoryComponent = character->GetInventoryComponent();
	if (!inventoryComponent)
	{
		K2_EndAbility();
		return;
	}

	if (inventoryComponent->HasItemEquipped())
	{
		inventoryComponent->UnEquipItem();
	}
}

void UAbilityEquip::CreateTaskPlayMontage(const FGameplayAbilityActorInfo* ActorInfo, UAnimMontage* montage)
{
	const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		montage,
		1.0,
		NAME_None,
		true);
	
	taskPlayMontage->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();
}

void UAbilityEquip::SubscribeToEventMontageItemBack()
{
	if(waitItemBackGameplayEventTask)
	{
		waitItemBackGameplayEventTask->EndTask();
	}
	
	waitItemBackGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_EQUIP_ITEM_BACK),
			nullptr,
			true);
	waitItemBackGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageItemBackReceived);
	waitItemBackGameplayEventTask->Activate();
}

bool UAbilityEquip::CanIHeal(const IICharacter* character) const
{
	const auto hurtComponent = character->GetHurtComponent();
	if (!hurtComponent)
		return false;

	if (hurtComponent->IsHealthFull())
		return false;

	const auto inventoryComponent = character->GetInventoryComponent();
	if (!inventoryComponent)
		return false;
	
	if (!inventoryComponent->HasItemOfType(InventoryItemHealStaticData))
		return false;
	
	return true;
}
