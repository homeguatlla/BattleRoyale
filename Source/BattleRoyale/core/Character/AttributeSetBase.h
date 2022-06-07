// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AttributeSetBase.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAttributeSetBase : public UAttributeSet
{
	GENERATED_BODY()

public:
	//TODO si el nombre de atributos crece igual vale la pena crear varias clases heredando de base
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRepHealth)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, Health)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRepMaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetBase, MaxHealth)

private:
	UFUNCTION()
	void OnRepHealth(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	void OnRepMaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
