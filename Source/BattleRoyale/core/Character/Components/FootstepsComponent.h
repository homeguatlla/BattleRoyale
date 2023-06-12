// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IFootstepsComponent.h"
#include "Components/ActorComponent.h"
#include "FootstepsComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UFootstepsComponent : public UActorComponent, public IIFootstepsComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFootstepsComponent();

	virtual void HandleFootstep(bool isRightFoot) const override;
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;
};
