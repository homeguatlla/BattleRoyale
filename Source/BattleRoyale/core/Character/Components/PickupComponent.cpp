// Fill out your copyright notice in the Description page of Project Settings.


#include "PickupComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values for this component's properties
UPickupComponent::UPickupComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}
/*
void UPickupComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UPickupComponent, mPickupObject);
}*/
