// Fill out your copyright notice in the Description page of Project Settings.


#include "FootstepNotify.h"

#include "BattleRoyale/core/Character/CharacterBase.h"

void UFootstepNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if(!MeshComp)
	{
		return;
	}
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

	const auto footstepComponent = character->GetFootstepsComponent();
	footstepComponent->HandleFootstep(IsRightFoot);
}
