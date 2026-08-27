// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "BattleRoyale/core/GameplayAbilitySystem/IAbilitySystemInterfaceBase.h"
#include "AbilityConsume.generated.h"

class UInventoryItemStaticData;

UCLASS()
class BATTLEROYALE_API UAbilityConsume : public UGameplayAbilityBase
{
	GENERATED_BODY()

	/** AnimMontage to play each time we heal */
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* ConsumeAnimation = nullptr;
	
public:
	UAbilityConsume();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	
	
private:
	void CreateTaskPlayMontageConsuming(const FGameplayAbilityActorInfo* ActorInfo);
	void SubscribeToEventMontageConsumeFinished();
	
	UFUNCTION()
	void OnMontageCompleted();
	
	UFUNCTION()
	void OnMontageCancelled();
	
	bool ApplyConsumeGameplayEffect(IIAbilitySystemInterfaceBase* abilitySystemComponent, UInventoryItemStaticData* inventoryItem);
	
	UFUNCTION()
	void OnEventMontageConsumeFinishedReceived(FGameplayEventData Payload);

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* waitConsumeFinishedGameplayEventTask = nullptr;
};
