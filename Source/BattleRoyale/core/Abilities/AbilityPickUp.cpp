// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityPickUp.h"

#include "BlueprintGameplayTagLibrary.h"
#include "GameplayTagsList.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "BattleRoyale/core/Character/Components/InventoryComponent.h"
#include "BattleRoyale/core/Character/Components/PickupSelectorComponent.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"

UAbilityPickUp::UAbilityPickUp()
{
	AbilityInputID = EAbilityInputID::PickUp;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerOnly;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_PICKUP));
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
		const auto inventoryComponent = character->GetInventoryComponent();
		check(inventoryComponent);
		const auto pickupSelectorComponent = character->GetPickupSelectorComponent();
		check(pickupSelectorComponent);
		
		//TODO podríamos quitar el GetPickableObject y hacer que el PickupObjectServer lo pille directamente
		//lo dejo por ahora porque igual el get nos podría hacer falta desde fuera del inventoryComponent.
		//TODO hay que tener en cuenta que si no tienes la habilidad de pickupindicator no vas a poder
		//recojer objetos. Hay que poner un else para que busque el objeto más cercano y lo devuelva o
		//quizá mejor acceder al PickupSelectorComponent y obtenga el objeto que esté marcado como
		//seleccionado
		if(inventoryComponent->PickupObjectServer(pickupSelectorComponent->GetSelectedPickableObject()))
		{
			CancelPickupIndicatorAbility(character);
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