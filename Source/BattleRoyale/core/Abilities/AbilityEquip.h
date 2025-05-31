// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "BattleRoyale/core/Character/Components/IInventoryComponent.h"
#include "AbilityEquip.generated.h"

class UInventoryItemStaticData;

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UAbilityEquip : public UGameplayAbilityBase
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UInventoryItemStaticData> InventoryItemHealStaticData;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* UnEquipAnimation = nullptr;
	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* EquipAnimation = nullptr;
	
public:
	UAbilityEquip();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

private:

	UFUNCTION()
	void OnMontageCancelled();
	UFUNCTION()
	void OnMontageCompleted();
	UFUNCTION()
	void OnEventMontageItemBackReceived(FGameplayEventData payload);
	
	void CreateTaskPlayMontage(const FGameplayAbilityActorInfo* ActorInfo, UAnimMontage* montage);
	
	void SubscribeToEventMontageItemBack();
	bool CanIHeal(const IICharacter* character) const;

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* mWaitItemBackGameplayEventTask = nullptr;

	TSubclassOf<UInventoryItemStaticData> mItemStaticDataToEquip = nullptr;
};
