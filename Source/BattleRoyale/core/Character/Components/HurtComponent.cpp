// Fill out your copyright notice in the Description page of Project Settings.


#include "HurtComponent.h"

#include <functional>
#include "AbilitySystemInterface.h"
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

void UHurtComponent::Initialize()
{
	const auto abilitySystemComponentInterface = GetAbilitySystemComponent();
	if(!abilitySystemComponentInterface)
	{
		return;
	}
	if(mHealthAttributes)
	{
		return;
	}

	const auto owner = Cast<ACharacterBase>(GetOwner());
	const auto owner2 = owner->GetPlayerState();
	mHealthAttributes = NewObject<UAttributeSetHealth>(owner2);
	
	abilitySystemComponentInterface->AddAttributeSet(mHealthAttributes);
	//const auto character = Cast<ACharacterBase>(GetOwner());
	//auto attributeHealth = character->GetAbilitySystemComponent()->GetAbilitySystemComponent()->GetSet<UAttributeSetHealth>();

	//TODO en este punto parece que el mHealthAttributes y el attributeHealth tienen la misma dirección de memoria
	//pero cuando hacemos el GetCurrentHealth no y por eso no funciona.
	//hay que revisar si el attribute set lo tiene tanto cliente como servidor (que es lo que parece)
	//y si el addattributeSet lo tienen que hacer ambos también, que supongo es lo correcto.
	//Revisar el código del GASShooter o el GASDocumentation uno de estos proyectos utiliza en c++ los atributos y se puede ver bien
	//que les funciona. Igual hay que hacer una prueba poniendo el attribute set dentro del playerstate
	
	//Play a gameplay effect to add the ability
	if(InitializeHurtEffect && GetOwner()->HasAuthority())
	{
		abilitySystemComponentInterface->ApplyGameplayEffectToSelf(InitializeHurtEffect);
	}
}

bool UHurtComponent::RegisterToHealthAttributeDelegate(std::function<void (const FOnAttributeChangeData& data)> callback) const
{
	const auto abilitySystemComponentInterface = GetAbilitySystemComponent();
	if(!abilitySystemComponentInterface || !mHealthAttributes)
	{
		return false;
	}
	
	auto& delegateOnHealthChanged = abilitySystemComponentInterface->GetAttributeValueChangeDelegate(mHealthAttributes->GetHealthAttribute());
	delegateOnHealthChanged.AddLambda(callback);
	
	return true;
}

void UHurtComponent::SetInvulnerableServer(bool isInvulnerable)
{
	ServerSetInvulnerable(isInvulnerable);
}

float UHurtComponent::GetCurrentHealth() const
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	if(!character)
	{
		return 0.0f;
	}
	const auto healthAttribute = character->GetAbilitySystemComponent()->GetAbilitySystemComponent()->GetSet<UAttributeSetHealth>();
	const auto h1 = healthAttribute->GetHealth();
	const auto h2 = mHealthAttributes->GetHealth();

	UE_LOG(LogTemp, Display, TEXT("character = %s h1 = %f, h2 = %f"), *character->GetName(), h1, h2);
	
	return healthAttribute->GetHealth();
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
