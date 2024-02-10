// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoMock.h"


// Sets default values
AAmmoMock::AAmmoMock()
{
	
}

void AAmmoMock::Initialize(EPickupObjectState state, int value)
{
	mState = state;
	mValue = value;
}

