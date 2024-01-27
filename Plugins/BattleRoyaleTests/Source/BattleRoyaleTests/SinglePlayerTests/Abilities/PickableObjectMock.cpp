// Fill out your copyright notice in the Description page of Project Settings.


#include "PickableObjectMock.h"


// Sets default values
APickableObjectMock::APickableObjectMock()
{
	
}

void APickableObjectMock::Initialize(const FName& name, int value, TSubclassOf<UInventoryItemStaticData> staticData)
{
	mName = name;
	mValue = value;
	mInventoryItemStaticData = staticData;
}