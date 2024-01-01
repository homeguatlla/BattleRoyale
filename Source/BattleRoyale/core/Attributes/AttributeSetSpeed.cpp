// Fill out your copyright notice in the Description page of Project Settings.

#include "AttributeSetSpeed.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayEffectExtension.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "Net/UnrealNetwork.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameMode.h"


UAttributeSetSpeed::UAttributeSetSpeed() :
MaxSpeed{400.0f}
{
}

void UAttributeSetSpeed::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetSpeed, MaxSpeed, COND_None, REPNOTIFY_Always);
}

void UAttributeSetSpeed::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	NewValue = FMath::Clamp<float>(NewValue, 0, MaxSpeed.GetBaseValue());
}

void UAttributeSetSpeed::OnRepMaxSpeed(const FGameplayAttributeData& OldMaxSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetSpeed, MaxSpeed, OldMaxSpeed);
}
