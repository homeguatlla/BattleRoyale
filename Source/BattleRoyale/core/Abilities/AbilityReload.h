﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "AbilityReload.generated.h"


class IGunComponent;
class IICharacter;

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilityReload : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityReload();
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:
	void CreateTaskPlayMontageReloading(const TScriptInterface<IGunComponent> combatComponent, const FGameplayAbilityActorInfo* ActorInfo);
	void SubscribeToEventMontageShoot(const IICharacter* character);
	
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnMontageCancelled();

	UFUNCTION()
	void OnEventMontageReloadReceived(const FGameplayEventData Payload);

	IICharacter* mCharacter;
	
	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* waitGameplayEventTask = nullptr;
};