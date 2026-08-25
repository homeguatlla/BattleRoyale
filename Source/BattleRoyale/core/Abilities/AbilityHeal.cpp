// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityHeal.h"
#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryItemStaticData.h"

UAbilityHeal::UAbilityHeal()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_HEAL));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_HEALING));

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));

	FAbilityTriggerData triggerDataToAdd;
	triggerDataToAdd.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_HEAL);
	triggerDataToAdd.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerDataToAdd);
}

bool UAbilityHeal::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
									  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
									  const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const auto character = GetCharacter(ActorInfo);
	if (!character)
		return false;
	
	const auto hurtComponent = character->GetHurtComponent();
	if (!hurtComponent)
		return false;

	if (hurtComponent->IsHealthFull())
		return false;
	
	const auto inventoryComponent = character->GetInventoryComponent();
	if (!inventoryComponent->HasItemEquipped())
		return false;

	const auto equippedItem = inventoryComponent->GetEquippedItem();
	const auto itemStaticData = equippedItem->GetInventoryItemStaticData();
	check(itemStaticData);
	
	return InventoryItemStaticData == itemStaticData;
}

void UAbilityHeal::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!HealAnimation)
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
	
	SubscribeToEventMontageHealFinished();
	CreateTaskPlayMontageHealing(ActorInfo);
}

void UAbilityHeal::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbilityHeal::CreateTaskPlayMontageHealing(const FGameplayAbilityActorInfo* ActorInfo)
{
	const auto sectionName = FName("Heal");
	const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		HealAnimation,
		1.0,
		sectionName,
		true);
	
	taskPlayMontage->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();
}

void UAbilityHeal::SubscribeToEventMontageHealFinished()
{
	if(waitHealFinishedGameplayEventTask)
	{
		waitHealFinishedGameplayEventTask->EndTask();
	}
	
	waitHealFinishedGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_HEAL_FINISHED),
			nullptr,
			true);
	waitHealFinishedGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageHealFinishedReceived);
	waitHealFinishedGameplayEventTask->Activate();
}

void UAbilityHeal::OnMontageCompleted()
{
	K2_EndAbility();
}

void UAbilityHeal::OnMontageCancelled()
{
	K2_CancelAbility();
}

void UAbilityHeal::OnEventMontageHealFinishedReceived(const FGameplayEventData Payload)
{
	const auto character = GetCharacter(GetAvatarActorFromActorInfo());
	if (!character)
		return;

	const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
	if (!abilitySystemComponent)
		return;

	if (!HealEffectClass)
		return;

	if (!InventoryItemStaticData)
		return;
	
	const auto inventoryItem = InventoryItemStaticData->GetDefaultObject<UInventoryItemStaticData>();
	if (!inventoryItem)
		return;
		
	if (!ApplyHealGameplayEffect(abilitySystemComponent, inventoryItem))
		return;
	
	const auto inventoryComponent = character->GetInventoryComponent();
	check(inventoryComponent);

	//TODO implementar el consume del elemento equipado
	//inventoryComponent->Consume(InventoryItemStaticData);
	
	//TODO agregar algun efecto visual al character a través de algún componente?
	
	K2_EndAbility();
}

bool UAbilityHeal::ApplyHealGameplayEffect(IIAbilitySystemInterfaceBase* const abilitySystemComponent, UInventoryItemStaticData* const inventoryItem)
{
	//Prepare a gameplay effect of type HealEffectClass to set using a TAG_DATA_HEAL_AMOUNT the amount of life to add
	//we need to prepare the GE_Heal to use the tag inside
	//Setting the Magnitude Calculation type = Set By Caller
	//and in the set by caller magnitude data tag = TAG_DATA_HEAL_AMOUNT
	const auto specHandle = MakeOutgoingGameplayEffectSpec(HealEffectClass, GetAbilityLevel());
	if (specHandle.IsValid())
	{
		const float healAmount = inventoryItem->GetValue();
		specHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TAG_DATA_HEAL_AMOUNT), healAmount);
		abilitySystemComponent->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get(), {});
		return true;
	}
	return false;
}