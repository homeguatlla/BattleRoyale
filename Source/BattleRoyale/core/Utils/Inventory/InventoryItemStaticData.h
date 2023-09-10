// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemStaticData.generated.h"


class APickupObjectBase;

UCLASS(Blueprintable, BlueprintType)
class BATTLEROYALE_API UInventoryItemStaticData : public UObject
{
	GENERATED_BODY()

public:
	
	UInventoryItemStaticData(){}
	UInventoryItemStaticData(const FName& name, TSubclassOf<APickupObjectBase> pickableObjectClass);
	UFUNCTION(BlueprintCallable)
	FName GetItemName() const { return mName; }
	TSubclassOf<APickupObjectBase> GetPickupObjectClass() const { return mPickupObjectClass;}
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUserWidget> GetItemWidgetClass() const { return mWidget; }
	UFUNCTION(BlueprintCallable)
	int GetValue() const { return mValue; }
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Name"))
	FName mName;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Pickup Object Class (Visual)"))
	TSubclassOf<APickupObjectBase> mPickupObjectClass;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="UI Widget (Visual)"))
	TSubclassOf<UUserWidget> mWidget = nullptr;
	
	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Can be equipped"))
	bool mCanBeEquipped = true;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName = "Value"))
	int mValue = 0;
};
