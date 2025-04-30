// Fill out your copyright notice in the Description page of Project Settings.


#include "HurtComponent.h"

#include <functional>
#include "GameplayEffectExtension.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/Attributes/AttributeSetHealth.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"

// Sets default values for this component's properties
UHurtComponent::UHurtComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
}

void UHurtComponent::BeginPlay()
{
	Super::BeginPlay();

	CacheAbilitySystemComponent();
}

void UHurtComponent::InitializeServer()
{
	if(!GetOwner()->HasAuthority())
	{
		return;
	}

	CacheAbilitySystemComponent();
	
	const auto owner = Cast<ACharacterBase>(GetOwner());
	const auto playerState = owner->GetPlayerState();
	
	mAbilitySystemComponent->AddAttributeSet(NewObject<UAttributeSetHealth>(playerState));

	//Play a gameplay effect to add the ability
	if(InitializeHurtEffect)
	{
		if(!mAbilitySystemComponent->ApplyGameplayEffectToSelf(InitializeHurtEffect).WasSuccessfullyApplied())
		{
			UE_LOG(LogCharacter, Warning, TEXT("[%s][UHurtComponent::InitializeServer] hurt initializing effect was not successfully applied"), *GetName());
		}
	}
}

bool UHurtComponent::RegisterToHealthAttributeDelegate(std::function<void (const FOnAttributeChangeData& data)> callback) const
{
	const auto attributeSetHealth = mAbilitySystemComponent->GetAttributeSetHealth();
	if(!attributeSetHealth)
	{
		return false;
	}
	
	auto& delegateOnHealthChanged = mAbilitySystemComponent->GetAttributeValueChangeDelegate(attributeSetHealth->GetHealthAttribute());
	delegateOnHealthChanged.AddLambda(callback);
	
	return true;
}

void UHurtComponent::SetInvulnerableServer(bool isInvulnerable)
{
	ServerSetInvulnerable(isInvulnerable);
}

float UHurtComponent::GetCurrentHealth() const
{
	check(mAbilitySystemComponent);
	
	if(const auto attributeSetHealth = mAbilitySystemComponent->GetAttributeSetHealth())
	{
		return attributeSetHealth->GetHealth();
	}
	
	ensureMsgf(false, TEXT("UHurtComponent::GetCurrentHealth error calling GetCurrentHealth before HurtComponent properly initialized and replicated"));
	return 0.0f;
}

bool UHurtComponent::IsCurrentHealthMax() const
{
	return false;
}

bool UHurtComponent::IsReady() const
{
	return mAbilitySystemComponent ? mAbilitySystemComponent->GetAttributeSetHealth() != nullptr : false;
}

void UHurtComponent::Dissolve() const
{
	check(mAbilitySystemComponent);
	if(!DissolveEffect)
	{
		return;
	}
	
	mAbilitySystemComponent->ApplyGameplayEffectToSelf(DissolveEffect);
}

IIAbilitySystemInterfaceBase* UHurtComponent::GetAbilitySystemComponent() const
{
	check(mAbilitySystemComponent);
	return mAbilitySystemComponent.GetInterface();
}

void UHurtComponent::ServerSetInvulnerable_Implementation(bool isInvulnerable)
{
	check(mAbilitySystemComponent);
	
	if(!isInvulnerable && mInvulnerableEffectHandle.IsValid())
	{
		mAbilitySystemComponent->RemoveGameplayEffect(mInvulnerableEffectHandle);
	}
	else if(isInvulnerable && !mInvulnerableEffectHandle.IsValid() && InvulnerableEffect)
	{
		mInvulnerableEffectHandle = mAbilitySystemComponent->ApplyGameplayEffectToSelf(InvulnerableEffect);
	}
}

void UHurtComponent::CacheAbilitySystemComponent()
{
	const auto character = Cast<ACharacterBase>(GetOwner());
	if (!character)
		return;
	
	const auto abilitySystemComponent = character->GetAbilitySystemComponentBase();
	if (!abilitySystemComponent || mAbilitySystemComponent)
		return ;
	
	mAbilitySystemComponent.SetInterface(abilitySystemComponent);
	mAbilitySystemComponent.SetObject(Cast<UObject>(abilitySystemComponent));
}