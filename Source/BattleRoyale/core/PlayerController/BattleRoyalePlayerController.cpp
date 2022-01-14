// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyalePlayerController.h"

void ABattleRoyalePlayerController::ClientEnableInput_Implementation(bool enable)
{
	SetIgnoreLookInput(!enable);
	SetIgnoreMoveInput(!enable);
}

void ABattleRoyalePlayerController::EnableInput(bool enable)
{
	ClientEnableInput(enable);
}

void ABattleRoyalePlayerController::BeginPlay()
{
	Super::BeginPlay();
}
