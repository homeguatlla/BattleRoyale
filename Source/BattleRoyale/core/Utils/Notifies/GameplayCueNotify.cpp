// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayCueNotify.h"

#include "AbilitySystemGlobals.h"
#include "GameplayCueManager.h"

void UGameplayCueNotify::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                     float TotalDuration)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration);

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				MeshComp->GetOwner(),
				GameplayCueTag,
				EGameplayCueEvent::Type::OnActive,
				FGameplayCueParameters());
	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				MeshComp->GetOwner(),
				GameplayCueTag,
				EGameplayCueEvent::Type::WhileActive,
				FGameplayCueParameters());
}

void UGameplayCueNotify::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::NotifyEnd(MeshComp, Animation);

	UAbilitySystemGlobals::Get().GetGameplayCueManager()->HandleGameplayCue(
				MeshComp->GetOwner(),
				GameplayCueTag,
				EGameplayCueEvent::Type::Removed,
				FGameplayCueParameters());
}
