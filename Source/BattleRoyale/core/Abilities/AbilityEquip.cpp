// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityEquip.h"

#include "GameplayTagsList.h"

UAbilityEquip::UAbilityEquip()
{
	AbilityInputID = EAbilityInputID::Equip;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::NonInstanced;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
	
	AbilityTags.AddTag(FGameplayTag::RequestGameplayTag(TAG_ABILITY_EQUIP));

	//TODO
	//Esta habilidad se tendría que activar cuando el pickup indicator lo indique, esté activo por ejemplo
	//y puede activar esta mediante el mismo sistema y pasándole el arma a activar
	//Para ello, tendríamos que enivar también el arma al pickup indicator.
	//Probar de pasarle el arma al pickup indicator y luego si funciona, entonces crear un objeto base o una interfaz
	//de objeto pickeable para que sea genérico. Creo que mejor una interfaz aunque luego habrá código repetido en los overlap
	//de todos los objetos. Darle unas vueltas a ver.
}

bool UAbilityEquip::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (!Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		return false;
	}

	return true;
}

void UAbilityEquip::ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString("AbilityEquip Activate"));
}

void UAbilityEquip::EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)
{
	GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Green, FString("AbilityEquip End"));
	
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

