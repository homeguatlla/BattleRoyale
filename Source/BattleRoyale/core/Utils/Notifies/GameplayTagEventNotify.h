// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagEventNotify.generated.h"

/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UGameplayTagEventNotify : public UAnimNotify
{
	GENERATED_BODY()
public:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	virtual FString GetNotifyName_Implementation() const override { return EventTag.GetTagName().ToString(); }
	
	UPROPERTY(BlueprintReadWrite, EditInstanceOnly, Category="Gameplay")
	FGameplayTag EventTag;
};
