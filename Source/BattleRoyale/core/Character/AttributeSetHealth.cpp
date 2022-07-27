// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetHealth.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"
#include "ICharacter.h"
#include "BattleRoyale/core/Abilities/GameplayTagsList.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameMode.h"


UAttributeSetHealth::UAttributeSetHealth() :
MaxHealth{100.0f},
Health{MaxHealth}
{
}

void UAttributeSetHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//TODO probar esto con un rep notify normal a ver que sucede
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAttributeSetHealth::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	NewValue = FMath::Clamp<float>(NewValue, 0, MaxHealth.GetBaseValue());
}

bool UAttributeSetHealth::PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data)
{
	const auto character = Cast<IICharacter>(GetOwningActor());
	const auto abilitySystemComponent = character->GetAbilitySystemComponent()->GetAbilitySystemComponent();
	if(!abilitySystemComponent)
	{
		return false;
	}

	if(!abilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(TAG_STATE_CAN_BE_HURT)))
	{
		return false;		
	}

	const auto instigatorPlayerState = Cast<IIPlayerState>(Data.EffectSpec.GetContext().GetInstigator());
	const auto receptorPlayerState = Cast<IIPlayerState>(abilitySystemComponent->GetOwner());

	if(instigatorPlayerState && receptorPlayerState)
	{
		const auto gameMode = GetGameModeServer();
		if(!gameMode->CanPlayerCauseDamageTo(instigatorPlayerState, receptorPlayerState))
		{
			return false;
		}
	}
	return Super::PreGameplayEffectExecute(Data);
}

void UAttributeSetHealth::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//The attribute changed by an gameplay effect is Health then
	if(Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<FProperty>(UAttributeSetHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetHealth, Health)))
	{
		UE_LOG(LogTemp, Display, TEXT("UAttributeSetBase::PostGameplayEffectExecute Health current value = %f"), Health.GetCurrentValue());
	
		if(IsAlive())
		{
			return;
		}
		
		const auto character = Cast<IICharacter>(GetOwningActor());
		const auto abilitySystemComponent = character->GetAbilitySystemComponent()->GetAbilitySystemComponent();
		if(!abilitySystemComponent)
		{
			return;
		}

		const auto instigatorPlayerState = Cast<IIPlayerState>(Data.EffectSpec.GetContext().GetInstigator());
		const auto receptorPlayerState = Cast<IIPlayerState>(abilitySystemComponent->GetOwner());

		if(instigatorPlayerState && receptorPlayerState)
		{
			const auto gameMode = GetGameModeServer();
			gameMode->OnNewKill(instigatorPlayerState, receptorPlayerState);
		}
	}
}

IIGameMode* UAttributeSetHealth::GetGameModeServer() const
{
	return Cast<IIGameMode>(GetWorld()->GetAuthGameMode<ABattleRoyaleGameMode>());
}

void UAttributeSetHealth::OnRepHealth(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, Health, OldHealth);
}

void UAttributeSetHealth::OnRepMaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, MaxHealth, OldMaxHealth);
}