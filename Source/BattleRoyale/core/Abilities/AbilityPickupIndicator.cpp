// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityPickupIndicator.h"

#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/TargetDatas/TargetDataPickupObject.h"
#include "Components/WidgetComponent.h"

UAbilityPickupIndicator::UAbilityPickupIndicator()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	//We want server initiated because we initiate from server (when a collision is detected)
	//and we want server replicates the activation with parameters to the client through a gameplay event.
	//otherwise, (serverOnly) the ability won't be activated on client to show the pickup indicator
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;

	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP_INDICATOR));
	
	//With this trigger we activate the ability if it receives an gameplay event with the event_pickup_indicator tag
	FAbilityTriggerData triggerData;
	triggerData.TriggerTag = FGameplayTag::RequestGameplayTag(TAG_EVENT_PICKUP_INDICATOR);
	triggerData.TriggerSource = EGameplayAbilityTriggerSource::GameplayEvent;
	AbilityTriggers.Add(triggerData);
}

bool UAbilityPickupIndicator::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UAbilityPickupIndicator::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                              const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                              const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	FTargetDataPickupObject pickupIndicatorData;
	
	if (!FillWithGameplayEventData(TriggerEventData, pickupIndicatorData))
	{
		K2_EndAbility();
		return;
	}

	if(pickupIndicatorData.mPickableObject->IsEquipped())
	{
		OnHidePickupIndicator();
		K2_EndAbility();
		return;
	}	
	
	if(ActorInfo->IsNetAuthority())
	{
		if(const auto character = Cast<IICharacter>(ActorInfo->AvatarActor))
		{
			character->SetPickupObject(pickupIndicatorData.mPickableObject);
		}
	}
	
	if(IsLocallyControlled())
	{
		OnShowPickupIndicator(pickupIndicatorData.mPickableObject, pickupIndicatorData.GetLocation());
		
		//character->GetAbilitySystemComponentBase()->GiveAbility();

		//TODO La tercera opción es que esta habilidad guarde el objeto detectado en el character, pickupComponent por ejemplo
		//de manera que el equip lo pueda tener. Pero entonces no necesitamos enviar tantas cosas, lo podemos guardar directamente
		//en el weapon base cuando detectamos el weapon en overlap? no, porque estaríamos poniendo lógica en el weapon overlap
		//mejor poner la lógica aquí.
		//Si es un objeto válido, etc... lo guardo en el pickupComponent. Hace falta el componente? o con una variable en el
		//character nos vale? 
		/*utils::UtilsLibrary::SendGameplayEventWithTargetData<FTargetDataPickupObject>(
			Cast<IICharacter>(character),
			FGameplayTag::RequestGameplayTag(TAG_EVENT_EQUIP),
			new FTargetDataPickupObject(FVector::Zero(), pickupIndicatorData.mPickableObject));*/
	}
}

void UAbilityPickupIndicator::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	if(ActorInfo->IsNetAuthority())
	{
		if(const auto character = Cast<IICharacter>(ActorInfo->AvatarActor))
		{
			character->SetPickupObject(nullptr);
		}
	}
	
	if(IsLocallyControlled())
	{
		OnHidePickupIndicator();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

bool UAbilityPickupIndicator::FillWithGameplayEventData(const FGameplayEventData* TriggerEventData, FTargetDataPickupObject& pickupIndicatorData) const
{
	if(TriggerEventData && TriggerEventData->TargetData.Num() > 0)
	{
		const FGameplayAbilityTargetData* targetData = TriggerEventData->TargetData.Data[0].Get();
		utils::UtilsLibrary::RetrieveGameplayEventTargetData<FTargetDataPickupObject>(TriggerEventData, pickupIndicatorData);
		return true;
	}
	return false;
}
