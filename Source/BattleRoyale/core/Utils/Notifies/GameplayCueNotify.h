// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayCueNotify.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UGameplayCueNotify : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	virtual void NotifyBegin(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent * MeshComp, UAnimSequenceBase * Animation, const FAnimNotifyEventReference& EventReference) override;

	virtual FString GetNotifyName_Implementation() const override { return GameplayCueTag.GetTagName().ToString(); }
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Gameplay Cue")
	FGameplayTag GameplayCueTag;
};
