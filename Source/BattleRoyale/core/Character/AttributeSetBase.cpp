// Fill out your copyright notice in the Description page of Project Settings.


#include "AttributeSetBase.h"
#include "Net/UnrealNetwork.h"


void UAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UAttributeSetBase, Health);
	DOREPLIFETIME(UAttributeSetBase, MaxHealth);
}

void UAttributeSetBase::OnRepHealth()
{
}

void UAttributeSetBase::OnRepMaxHealth()
{
	
}