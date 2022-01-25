// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBase.h"

// Sets default values
AWeaponBase::AWeaponBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	//bReplicates = true;

	// Create a gun mesh component
	mMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Gun"));
	mMesh->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	mMesh->bCastDynamicShadow = false;
	mMesh->CastShadow = false;
}

// Called when the game starts or when spawned
void AWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
}
