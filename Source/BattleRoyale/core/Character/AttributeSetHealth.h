// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AttributeSet.h"
#include "AttributeSetHealth.generated.h"

// Uses macros from AttributeSet.h
#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

class IIGameMode;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAttributeSetHealth : public UAttributeSet
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRepMaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, MaxHealth)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, ReplicatedUsing = OnRepHealth)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UAttributeSetHealth, Health)

	UAttributeSetHealth();
	
private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;
	IIGameMode* GetGameModeServer() const;
	bool IsAlive() const { return Health.GetCurrentValue() > 0.0f;}

	UFUNCTION()
	void OnRepHealth(const FGameplayAttributeData& OldHealth);
	UFUNCTION()
	void OnRepMaxHealth(const FGameplayAttributeData& OldMaxHealth);
};
