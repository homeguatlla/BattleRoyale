// Fill out your copyright notice in the Description page of Project Settings.

#include "GameplayTagEventNotify.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

void UGameplayTagEventNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	const auto owner = MeshComp->GetOwner();

	if(!owner)
	{
		return;
	}

	const auto character = Cast<ACharacterBase>(owner);

	if(!character)
	{
		return;
	}

	const auto playerState = Cast<APlayerStateBase>(character->GetPlayerState());
	if(!playerState)
	{
		return;
	}
	
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(playerState, EventTag, FGameplayEventData());
}
