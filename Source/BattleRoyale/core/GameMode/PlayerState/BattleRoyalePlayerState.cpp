// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyalePlayerState.h"

#include "AbilitySystemComponent.h"

ABattleRoyalePlayerState::ABattleRoyalePlayerState()
{
	mAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	mAbilitySystemComponent->SetIsReplicated(true);
}
