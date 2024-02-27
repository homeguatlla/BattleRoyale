// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityReload.h"

#include "AbilityShootProjectileGun.h"
#include "GameplayTagsList.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/IGunComponent.h"
#include "BattleRoyale/core/PickableObjects/Weapons/WeaponBase.h"

UAbilityReload::UAbilityReload()
{
	AbilityInputID = EAbilityInputID::Reload;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_RELOAD));
	ActivationOwnedTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_STATE_RELOADING));

	BlockAbilitiesWithTag.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_SHOOT_PROJECTILE));
}

bool UAbilityReload::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	const auto character = GetCharacter(ActorInfo);
	if (!character)
	{
		return false;
	}

	const auto combatComponent = character->GetGunComponent();
	const auto inventoryComponent = character->GetInventoryComponent();
	if(!combatComponent || !inventoryComponent)
	{
		return false;
	}

	return combatComponent->CanReload(inventoryComponent);
}

void UAbilityReload::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	mCharacter = GetCharacter(ActorInfo);
	if (mCharacter)
	{
		if(const auto combatComponent = mCharacter->GetGunComponent())
		{
			SubscribeToEventMontageReload(mCharacter);
			SubscribeToEventMontageAmmoIn(mCharacter);
			SubscribeToEventMontageAmmoOut(mCharacter);
			CreateTaskPlayMontageReloading(combatComponent, ActorInfo);
			combatComponent->ReleaseTrigger();
			return;
		}
	}
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbilityReload::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UAbilityReload::CreateTaskPlayMontageReloading(const TScriptInterface<IGunComponent> combatComponent, const FGameplayAbilityActorInfo* ActorInfo)
{
	const auto sectionName = FName("Reload");
	const auto taskPlayMontage = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		NAME_None,
		combatComponent->GetReloadingMontage(),
		1.0,
		sectionName,
		true);
	
	taskPlayMontage->OnCancelled.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageCancelled);
	taskPlayMontage->OnCompleted.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageCompleted);
	taskPlayMontage->ReadyForActivation();
}

void UAbilityReload::SubscribeToEventMontageAmmoIn(const IICharacter* character)
{
	if(waitAmmoInGameplayEventTask)
	{
		waitAmmoInGameplayEventTask->EndTask();
	}
	
	waitAmmoInGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_RELOAD_AMMO_IN),
			nullptr,
			true);
	waitAmmoInGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageAmmoInReceived);
	waitAmmoInGameplayEventTask->Activate();
}

void UAbilityReload::SubscribeToEventMontageAmmoOut(const IICharacter* character)
{
	if(waitAmmoOutGameplayEventTask)
	{
		waitAmmoOutGameplayEventTask->EndTask();
	}
	
	waitAmmoOutGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_RELOAD_AMMO_OUT),
			nullptr,
			true);
	waitAmmoOutGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageAmmoOutReceived);
	waitAmmoOutGameplayEventTask->Activate();
}

void UAbilityReload::SubscribeToEventMontageReload(const IICharacter* character)
{
	if(waitReloadGameplayEventTask)
	{
		waitReloadGameplayEventTask->EndTask();
	}
	
	waitReloadGameplayEventTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
			this,
			FGameplayTag::RequestGameplayTag(TAG_EVENT_MONTAGE_RELOAD_RELOAD),
			nullptr,
			true);
	waitReloadGameplayEventTask->EventReceived.AddDynamic(this, &ThisClass::OnEventMontageReloadReceived);
	waitReloadGameplayEventTask->Activate();
}

void UAbilityReload::OnMontageCompleted()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
}

void UAbilityReload::OnMontageCancelled()
{
	EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, true);
}

void UAbilityReload::OnEventMontageReloadReceived(const FGameplayEventData Payload)
{
	if(const auto inventoryComponent = mCharacter->GetInventoryComponent())
	{
		const auto combatComponent = mCharacter->GetGunComponent();
		if(combatComponent->CanReload(inventoryComponent))
		{
			combatComponent->Reload(inventoryComponent);
		}	
	}
}

void UAbilityReload::OnEventMontageAmmoInReceived(const FGameplayEventData Payload)
{
	const auto combatComponent = mCharacter->GetGunComponent();
	const auto weapon = Cast<AWeaponBase>(combatComponent->GetEquippedWeapon().GetObject());
	weapon->BP_OnReloadAmmoIn();
}

void UAbilityReload::OnEventMontageAmmoOutReceived(const FGameplayEventData Payload)
{
	const auto combatComponent = mCharacter->GetGunComponent();
	const auto weapon = Cast<AWeaponBase>(combatComponent->GetEquippedWeapon().GetObject());
	weapon->BP_OnReloadAmmoOut();
}
