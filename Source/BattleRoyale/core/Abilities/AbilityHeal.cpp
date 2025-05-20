// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityHeal.h"
#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/HurtComponent.h"
#include "BattleRoyale/core/Character/Components/IGunComponent.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"

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

	return inventoryComponent->HasItemOfType(InventoryItemStaticData);
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
	
	//TODO 1) probablemente lo ideal sería si tienes arma equipada, guardar-la ejecutando animación de unequip
	//curarse, y luego reequipar el arma.
	if (const auto combatComponent = character->GetGunComponent())
	{
		if (combatComponent->HasWeaponEquipped())
		{
			//TODO esto hay que pensarlo bien, porque requiere de lanzar una animación y esto no lo puede hacer el combat component porque
			//no lanza animaciones. Así que probablemente requiera de una habilidad. Y de alguna manera de saber que la habilidad a terminado
			//quizá esperando un evento.
			
			//combatComponent->PutAwayEquippedWeapon();
		}
	}
	
	//Si no tienes arma tal cual.
	//2) Además habría que poner el battery cell en la mano antes de ejecutar la animación
	//3) Eliminar la cell del inventario.
	
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
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbilityHeal::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAbilityHeal::OnEventMontageHealFinishedReceived(const FGameplayEventData Payload)
{
	const auto character = GetCharacter(GetAvatarActorFromActorInfo());
	if (!character)
		return;

	const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
	if (!abilitySystemComponent)
		return;
	
	abilitySystemComponent->ApplyGameplayEffectToSelf(HealEffectClass);
	
	const auto inventoryComponent = character->GetInventoryComponent();
	check(inventoryComponent);
	
	//inventoryComponent->Consume(InventoryItemStaticData);
	
	//TODO agregar algun efecto visual al character a través de algún componente?
}