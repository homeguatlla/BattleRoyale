
#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilityBase.h"
#include "Abilities/GameplayAbility.h"
#include "AbilityShootProjectileGun.generated.h"

/**
 * Makes the Character try to sprint. This is an example of a non-instanced ability.
 */
UCLASS()
class UAbilityShootProjectileGun : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	UAbilityShootProjectileGun();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	                           const FGameplayAbilityActivationInfo ActivationInfo) const override;

private:
	UFUNCTION()
	void OnMontageCompleted();

	UFUNCTION()
	void OnMontageCancelled();
	void Shoot() const;

	UFUNCTION()
	void OnEventMontageShootReceived(const FGameplayEventData Payload);
	
	FGameplayTagContainer GetWeaponCooldownGameplayTags() const;
	float GetWeaponCooldownDuration() const;
	void SubscribeToEventMontageShoot(const IICharacter* character);
	void CreateTaskPlayMontageShooting(const IICharacter* character, const FGameplayAbilityActorInfo* ActorInfo);
	void OnStopShooting(FGameplayTag gameplayTag, const FGameplayEventData* payload);
	
	IICharacter* mCharacter;
	//METHOD FDelegateHandle mEventMontageShootHandle;

	UPROPERTY(Transient)
	FGameplayTagContainer CooldownTagContainer;

	UPROPERTY()
	class UAbilityTask_WaitGameplayEvent* waitGameplayEventTask;

	FDelegateHandle m_StopShootingHandle;
};
