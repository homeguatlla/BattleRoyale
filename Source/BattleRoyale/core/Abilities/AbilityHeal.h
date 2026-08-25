// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "AbilityHeal.generated.h"

class UInventoryItemStaticData;

UCLASS()
class BATTLEROYALE_API UAbilityHeal : public UGameplayAbilityBase
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemStaticData> InventoryItemStaticData = nullptr;
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UGameplayEffect> HealEffectClass = nullptr;
	/** AnimMontage to play each time we heal */
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* HealAnimation = nullptr;
	
public:
	UAbilityHeal();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
private:
	void CreateTaskPlayMontageHealing(const FGameplayAbilityActorInfo* ActorInfo);
	void SubscribeToEventMontageHealFinished();
	
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageCancelled();
	
	bool ApplyHealGameplayEffect(IIAbilitySystemInterfaceBase* abilitySystemComponent, UInventoryItemStaticData* inventoryItem);
	
	UFUNCTION()
	void OnEventMontageHealFinishedReceived(FGameplayEventData Payload);

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* waitHealFinishedGameplayEventTask = nullptr;
};
