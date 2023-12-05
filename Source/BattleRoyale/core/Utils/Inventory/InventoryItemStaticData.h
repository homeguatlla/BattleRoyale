// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemStaticData.generated.h"


class APickableObjectBase;

UCLASS(Blueprintable, BlueprintType)
class BATTLEROYALE_API UInventoryItemStaticData : public UObject
{
	GENERATED_BODY()

public:
	
	UInventoryItemStaticData(){}
	UInventoryItemStaticData(const FName& name, TSubclassOf<APickableObjectBase> pickableObjectClass);
	UFUNCTION(BlueprintCallable)
	FName GetItemName() const { return mName; }
	TSubclassOf<APickableObjectBase> GetPickupObjectClass() const { return mPickupObjectClass;}
	UFUNCTION(BlueprintCallable)
	TSubclassOf<UUserWidget> GetItemWidgetClass() const { return mWidget; }
	UFUNCTION(BlueprintCallable)
	int GetValue() const { return mValue; }
	UFUNCTION(Blueprintable)
	bool CanBeEquipped() const { return mCanBeEquipped; }
	
private:
	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Name"))
	FName mName;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Pickup Object Class (Visual)"))
	TSubclassOf<APickableObjectBase> mPickupObjectClass;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="UI Widget (Visual)"))
	TSubclassOf<UUserWidget> mWidget = nullptr;
	
	UPROPERTY(EditDefaultsOnly, meta=(DisplayName="Can be equipped"))
	bool mCanBeEquipped = false;

	UPROPERTY(EditDefaultsOnly, meta=(DisplayName = "Value"))
	int mValue = 0;

	//TODO maybe here or in a subclass
	//we can add new abilities, so could be a property array of GameplayAbility.
};
