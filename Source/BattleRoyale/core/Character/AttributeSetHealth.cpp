// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetHealth.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"


void UAttributeSetHealth::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//TODO probar esto con un rep notify normala ver que sucede
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetHealth, MaxHealth, COND_None, REPNOTIFY_Always);
}

void UAttributeSetHealth::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	//The attribute changed by an gameplay effect is Health then
	if(Data.EvaluatedData.Attribute.GetUProperty() == FindFieldChecked<UProperty>(UAttributeSetHealth::StaticClass(), GET_MEMBER_NAME_CHECKED(UAttributeSetHealth, Health)))
	{
		UE_LOG(LogTemp, Display, TEXT("UAttributeSetBase::PostGameplayEffectExecute Health current value = %f"), Health.GetCurrentValue());
	}
}

void UAttributeSetHealth::OnRepHealth(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, Health, OldHealth);
}

void UAttributeSetHealth::OnRepMaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetHealth, MaxHealth, OldMaxHealth);
}