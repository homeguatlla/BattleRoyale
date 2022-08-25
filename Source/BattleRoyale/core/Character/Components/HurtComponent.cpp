// Fill out your copyright notice in the Description page of Project Settings.


#include "HurtComponent.h"

#include <functional>
#include "GameplayEffectExtension.h"
#include "BattleRoyale/core/Character/AttributeSetHealth.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UHurtComponent::UHurtComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UHurtComponent::InitializeServer()
{
	if(!GetOwner()->HasAuthority())
	{
		return;
	}
	
	const auto abilitySystemComponentInterface = GetAbilitySystemComponent();
	if(!abilitySystemComponentInterface)
	{
		return;
	}
	
	const auto owner = Cast<ACharacterBase>(GetOwner());
	const auto playerState = owner->GetPlayerState();
	
	abilitySystemComponentInterface->AddAttributeSet(NewObject<UAttributeSetHealth>(playerState));

	//Play a gameplay effect to add the ability
	if(InitializeHurtEffect)
	{
		abilitySystemComponentInterface->ApplyGameplayEffectToSelf(InitializeHurtEffect);
	}
}

bool UHurtComponent::RegisterToHealthAttributeDelegate(std::function<void (const FOnAttributeChangeData& data)> callback) const
{
	const auto abilitySystemComponentInterface = GetAbilitySystemComponent();
	if(!abilitySystemComponentInterface)
	{
		return false;
	}

	const auto attributeSetHealth = abilitySystemComponentInterface->GetAttributeSetHealth();
	if(!attributeSetHealth)
	{
		return false;
	}
	
	auto& delegateOnHealthChanged = abilitySystemComponentInterface->GetAttributeValueChangeDelegate(attributeSetHealth->GetHealthAttribute());
	delegateOnHealthChanged.AddLambda(callback);
	
	return true;
}

void UHurtComponent::SetInvulnerableServer(bool isInvulnerable)
{
	ServerSetInvulnerable(isInvulnerable);
}

float UHurtComponent::GetCurrentHealth() const
{
	const auto abilitySystemComponentInterface = GetAbilitySystemComponent();
	if(abilitySystemComponentInterface)
	{
		const auto attributeSetHealth = abilitySystemComponentInterface->GetAttributeSetHealth();
		if(attributeSetHealth)
		{
			return attributeSetHealth->GetHealth();
		}
	}

	ensureMsgf(false, TEXT("UHurtComponent::GetCurrentHealth error calling GetCurrentHealth before HurtComponent properly initialized and replicated"));
	return 0.0f;
}

bool UHurtComponent::IsReady() const
{
	auto abilitySystem = GetAbilitySystemComponent();

	return abilitySystem->GetAttributeSetHealth() != nullptr;
}

IIAbilitySystemInterfaceBase* UHurtComponent::GetAbilitySystemComponent() const
{
	const auto character = Cast<IICharacter>(GetOwner());
	if(!character)
	{
		return nullptr;
	}

	return character->GetAbilitySystemComponentBase();
}

void UHurtComponent::ServerSetInvulnerable_Implementation(bool isInvulnerable)
{
	const auto abilitySystemComponentInterface = GetAbilitySystemComponent();
	if(!abilitySystemComponentInterface)
	{
		return;
	}
	if(isInvulnerable && mVulnerableEffectHandle.IsValid())
	{
		abilitySystemComponentInterface->RemoveGameplayEffect(mVulnerableEffectHandle);
	}
	else if(!isInvulnerable)
	{
		mVulnerableEffectHandle = abilitySystemComponentInterface->ApplyGameplayEffectToSelf(VulnerableEffect);
	}
}
