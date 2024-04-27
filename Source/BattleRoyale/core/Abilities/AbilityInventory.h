// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "AbilityInventory.generated.h"

struct FGameplayEventData;

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilityInventory : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityInventory();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

private:
	TScriptInterface<IIInventoryComponent> GetInventoryComponent(const FGameplayAbilityActorInfo* ActorInfo);
	void OnHideInventory(FGameplayTag gameplayTag, const FGameplayEventData* payload);
	
	FDelegateHandle m_HideInventoryHandle;
};
