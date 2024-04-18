// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "IAbilitySystemInterfaceBase.h"
#include "AbilitySystemComponentBase.generated.h"

class UAttributeSetHealth;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilitySystemComponentBase : public UAbilitySystemComponent, public IIAbilitySystemInterfaceBase
{
	GENERATED_BODY()

public:

	virtual void SetSimulatedMontage(UAnimMontage* montage) override;

	virtual FActiveGameplayEffectHandle ApplyGameplayEffectToTarget(const TSubclassOf<UGameplayEffect>& effect, const IICharacter* target) override;
	virtual FActiveGameplayEffectHandle ApplyGameplayEffectToSelf(const TSubclassOf<UGameplayEffect>& effectClass) override;
	virtual bool RemoveGameplayEffect(FActiveGameplayEffectHandle handle) override;

	//Add an attribute set to the gameplay ability component. It is used when attribute sets are in a different class than the gameplayabilitycompoent.
	virtual void AddAttributeSet(UAttributeSet* attributeSet) override;

	virtual const UAttributeSetHealth* GetAttributeSetHealth() const override;
	virtual const UAttributeSetSpeed* GetAttributeSetSpeed() const override;
	
	virtual FOnGameplayAttributeValueChange& GetAttributeValueChangeDelegate(FGameplayAttribute attribute) override;

	virtual bool HasGameplayTag(const FGameplayTag& tag) const override;
	virtual void AddGameplayTag(const FGameplayTag& tag) override;
	virtual void RemoveGameplayTag(const FGameplayTag& tag) override;
	
	virtual void SendGameplayEvent(const FGameplayTag& tag, const FGameplayEventData& payLoad) override;
	virtual void SendGameplayEvent(const FGameplayTag& tag, const AActor* instigator) override;

	virtual void CancelAbilitiesWithTags(const FGameplayTagContainer& tags) override;
	
	/*virtual float PlayMontage(UGameplayAbility* AnimatingAbility, FGameplayAbilityActivationInfo ActivationInfo, UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None, float StartTimeSeconds = 0.0f) override;
	virtual float PlayMontageSimulated(UAnimMontage* Montage, float InPlayRate, FName StartSectionName = NAME_None) override;*/
};
