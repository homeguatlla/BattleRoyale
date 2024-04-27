// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "AbilityAim.generated.h"

class IGunComponent;

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilityAim : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityAim();
	virtual void ActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                             FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
							const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags,
							FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void CancelAbility(FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
					   FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;
private:
	TScriptInterface<IGunComponent> GetGunComponent(const FGameplayAbilityActorInfo* ActorInfo) const;
	void OnStopAiming(FGameplayTag gameplayTag, const FGameplayEventData* payload);

	FDelegateHandle m_StopAimingHandle;
};
