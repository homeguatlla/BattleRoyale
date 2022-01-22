// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerControllerBase.h"

void APlayerControllerBase::ClientEnableInput_Implementation(bool enable)
{
	SetIgnoreLookInput(!enable);
	SetIgnoreMoveInput(!enable);
}

void APlayerControllerBase::EnableInput(bool enable)
{
	ClientEnableInput(enable);
}

void APlayerControllerBase::BeginPlay()
{
	Super::BeginPlay();
}
