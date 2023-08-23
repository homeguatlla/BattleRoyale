// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemInstance.generated.h"

class IPickupObject;
class UInventoryItemStaticData;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BATTLEROYALE_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()
	
	virtual bool IsSupportedForNetworking() const override { return true; }

public:
	void Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass);
	const UInventoryItemStaticData* GetStaticData() const;

	//OnEquip means to put it on hand
	void OnEquipped();
	//OnUnEquip means to put it back to the inventory
	void OnUnEquipped();

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_IsEquipped();

	UPROPERTY(Replicated)
	TSubclassOf<UInventoryItemStaticData> mStaticDataClass;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipped)
	bool mIsEquipped = false;
};
