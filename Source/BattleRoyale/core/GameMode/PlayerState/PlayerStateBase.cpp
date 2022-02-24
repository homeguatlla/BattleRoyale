// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

APlayerStateBase::APlayerStateBase()
{
	mAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	mAbilitySystemComponent->SetIsReplicated(true);
}
