// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameplayEffect.h"
#include "Abilities/GameplayAbilityTypes.h"
#include "IAbilitySystemInterfaceBase.generated.h"

class UAbilitySystemComponentBase;
class IICharacter;
class UAttributeSetHealth;

// This class does not need to be modified.
UINTERFACE()
class UIAbilitySystemInterfaceBase : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIAbilitySystemInterfaceBase
{
	GENERATED_BODY()

public:
	virtual void SetSimulatedMontage(UAnimMontage* montage) = 0;

	virtual FActiveGameplayEffectHandle ApplyGameplayEffectToTarget(const TSubclassOf<UGameplayEffect>& effect, const IICharacter* target) = 0;
	virtual FActiveGameplayEffectHandle ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& effectClass) = 0;
	virtual bool RemoveGameplayEffect(FActiveGameplayEffectHandle handle) = 0;
	
	virtual void AddAttributeSet(UAttributeSet* attributeSet) = 0;
	virtual const UAttributeSetHealth* GetAttributeSetHealth() const = 0;
	virtual FOnGameplayAttributeValueChange& GetAttributeValueChangeDelegate(FGameplayAttribute Attribute) = 0;
	
	virtual bool HasGameplayTag(const FGameplayTag& tag) const  = 0;

	virtual void SendGameplayEvent(const FGameplayTag& tag, const FGameplayEventData& payLoad) = 0;

	virtual void CancelAbilitiesWithTags(const FGameplayTagContainer& tags) = 0;
};
