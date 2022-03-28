// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleRoyale/core/Character/ICharacter.h"

APlayerStateBase::APlayerStateBase() : mTeamId(0)
{
	mAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	mAbilitySystemComponent->SetIsReplicated(true);
}

bool APlayerStateBase::IsAlive() const
{
	if(GetPawn()->Implements<UICharacter>())
	{
		//TODO quizá podríamos pillar la vida del GAS cuando sea un atributo
		const auto character = Cast<IICharacter>(GetPawn());
		return character->IsAlive();
	}
	
	return false;
}
