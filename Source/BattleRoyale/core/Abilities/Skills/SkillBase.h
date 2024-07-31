// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/Abilities/GameplayAbilityBase.h"
#include "BattleRoyale/core/HUD/Widgets/Skills/SkillWidget.h"
#include "SkillBase.generated.h"

UCLASS()
class BATTLEROYALE_API USkillBase : public UGameplayAbilityBase
{
	GENERATED_BODY()

public:
	USkillBase();
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;

	TSubclassOf<USkillWidget> GetWidgetHUD() const { return WidgetHUD; }
	float GetCooldownDuration() const;
	
private:
	void NotifyOnStartSkillCooldown(const TWeakObjectPtr<AActor> avatarActor);
	void NotifyOnStopSkillCooldown(const TWeakObjectPtr<AActor> avatarActor);
	

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USkillWidget> WidgetHUD;
};
