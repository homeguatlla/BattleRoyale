// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/General/IPickupObject.h"
#include "Components/ActorComponent.h"
#include "PickupComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UPickupComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPickupComponent();
	void SetPickupObject(TScriptInterface<IPickupObject> object) { mPickupObject = object; }
	TScriptInterface<IPickupObject> GetPickupObject() const { return mPickupObject; }
	
private:
	TScriptInterface<IPickupObject> mPickupObject;
};
