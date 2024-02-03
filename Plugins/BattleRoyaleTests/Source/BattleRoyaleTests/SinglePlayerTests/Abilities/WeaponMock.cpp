// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponMock.h"


// Sets default values
AWeaponMock::AWeaponMock()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AWeaponMock::Initialize(EPickupObjectState state, int ammo)
{
	mState = state;
	mAmmo = ammo;
}
