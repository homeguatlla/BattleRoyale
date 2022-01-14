// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "IPlayerController.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UIPlayerController : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class BATTLEROYALE_API IIPlayerController
{
	GENERATED_BODY()
	
public:
	virtual void EnableInput(bool enable) = 0;
};
