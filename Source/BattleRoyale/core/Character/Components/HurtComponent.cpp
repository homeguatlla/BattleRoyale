// Fill out your copyright notice in the Description page of Project Settings.


#include "HurtComponent.h"

#include <functional>
#include <vcruntime_startup.h>

#include "AbilitySystemInterface.h"
#include "GameplayEffectExtension.h"
#include "BattleRoyale/core/Character/AttributeSetHealth.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
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
	
	mHealthAttributes = NewObject<UAttributeSetHealth>(this);
	abilitySystemComponentInterface->AddAttributeSet(mHealthAttributes);
	
	//Play a gameplay effect to add the ability
	if(InitializeHurtEffect)
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
	
	auto& delegateOnHealthChanged = abilitySystemComponentInterface->GetGameplayAttributeValueChangeDelegate_(mHealthAttributes->GetHealthAttribute());
	delegateOnHealthChanged.AddLambda(callback);
	
	return true;
}

void UHurtComponent::TakeDamageServer(float damage, APlayerController* instigator, APlayerController* hurt)
{
	/*UE_LOG(LogCharacter, Warning, TEXT("ACharacterBase::TakeDamage Server"));
	const auto newHealth = mGameplayAbilityAttributes->GetHealth() - actualDamage;
	mDamageCauser.health = FMath::Clamp(newHealth, 0.0f, MaxHealth);
	mDamageCauser.damage = actualDamage;
	mDamageCauser.playerCauser = EventInstigator->GetCharacter();
	//the replication is not received by server so, we need to update it here
	UpdateHealth(mDamageCauser);

	//This is ok here because only server can increase the number of kills.
	if(!mGameplayAbilityAttributes->IsAlive())
	{
		gameMode->OnNewKill(killer,	victim);
	}*/
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