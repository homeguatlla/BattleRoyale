// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityConsume.h"
#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryItemStaticData.h"

UAbilityConsume::UAbilityConsume()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_CONSUME));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_CONSUMING));

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));

	FAbilityTriggerData triggerDataToAdd;
	triggerDataToAdd.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_INPUT_CONSUME);
	triggerDataToAdd.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerDataToAdd);
}

bool UAbilityConsume::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
									  const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
									  const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
		return false;

	const auto character = GetCharacter(ActorInfo);
	if (!character)
		return false;
	
	const auto inventoryComponent = character->GetInventoryComponent();
	if (!inventoryComponent->HasItemEquipped())
		return false;

	const auto equippedItem = inventoryComponent->GetEquippedItem();
	
	return equippedItem->CanBeConsumed();
}

void UAbilityConsume::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (!ConsumeAnimation)
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
	
	SubscribeToEventMontageConsumeFinished();
	CreateTaskPlayMontageConsuming(ActorInfo);
}

void UAbilityConsume::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbilityConsume::CreateTaskPlayMontageConsuming(const FGameplayAbilityActorInfo* ActorInfo)
{
	const auto sectionName = FName("Consume");
	const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		ConsumeAnimation,
		1.0,
		sectionName,
		true);
	
	taskPlayMontage->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();
}

void UAbilityConsume::SubscribeToEventMontageConsumeFinished()
{
	if(waitConsumeFinishedGameplayEventTask)
	{
		waitConsumeFinishedGameplayEventTask->EndTask();
	}
	
	waitConsumeFinishedGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_CONSUME_FINISHED),
			nullptr,
			true);
	waitConsumeFinishedGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageConsumeFinishedReceived);
	waitConsumeFinishedGameplayEventTask->Activate();
}

void UAbilityConsume::OnMontageCompleted()
{
	K2_EndAbility();
}

void UAbilityConsume::OnMontageCancelled()
{
	K2_CancelAbility();
}

void UAbilityConsume::OnEventMontageConsumeFinishedReceived(const FGameplayEventData Payload)
{
	const auto character = GetCharacter(GetAvatarActorFromActorInfo());
	if (!character)
		return;

	const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
	if (!abilitySystemComponent)
		return;
	
	const auto inventoryComponent = character->GetInventoryComponent();
	check(inventoryComponent);
	
	if (!inventoryComponent->HasItemEquipped())
		return;
	
	const auto itemEquipped = inventoryComponent->GetEquippedItem();
	if (!itemEquipped)
		return;
	
	const auto inventoryItem = itemEquipped->GetInventoryItemStaticData()->GetDefaultObject<UInventoryItemStaticData>();
	if (!inventoryItem)
		return;
		
	if (!ApplyConsumeGameplayEffect(abilitySystemComponent, inventoryItem))
		return;
	
	
	
	//TODO implementar el consume del elemento equipado que se supone que está en la mano. Igual podríamos hacer que lo tira.
	inventoryComponent->ConsumeEquippedItem();
	
	//TODO agregar algun efecto visual al character a través de algún componente?
	
	K2_EndAbility();
}

bool UAbilityConsume::ApplyConsumeGameplayEffect(IIAbilitySystemInterfaceBase* const abilitySystemComponent, UInventoryItemStaticData* const inventoryItem)
{
	//Prepare a gameplay effect of type ConsumeEffectClass to set using a TAG_DATA_CONSUME_AMOUNT the amount of whatever to add
	//we need to prepare the GE_Heal for heal or another one to use the tag inside
	//Setting the Magnitude Calculation type = Set By Caller
	//and in the set by caller magnitude data tag = TAG_DATA_CONSUME_AMOUNT
	const auto specHandle = MakeOutgoingGameplayEffectSpec(inventoryItem->GetEffectOnConsume(), GetAbilityLevel());
	if (specHandle.IsValid())
	{
		const float amount = inventoryItem->GetValue();
		specHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(TAG_DATA_CONSUME_AMOUNT), amount);
		const auto activeEffectHandle = abilitySystemComponent->ApplyGameplayEffectSpecToSelf(*specHandle.Data.Get(), {});
		return activeEffectHandle.WasSuccessfullyApplied();
	}
	return false;
}