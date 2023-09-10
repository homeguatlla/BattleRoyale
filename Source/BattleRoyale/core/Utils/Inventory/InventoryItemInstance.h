// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInventoryItemInstance.h"
#include "UObject/Object.h"
#include "InventoryItemInstance.generated.h"

class IPickupObject;
class UInventoryItemStaticData;
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class BATTLEROYALE_API UInventoryItemInstance : public UObject, public IIInventoryItemInstance
{
	GENERATED_BODY()
	
	virtual bool IsSupportedForNetworking() const override { return true; }

public:
	virtual void Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int value) override;
	virtual const UInventoryItemStaticData* GetStaticData() const override;
	virtual int GetValue() const override { return mValue; }
	
	//OnEquip means to put it on hand
	virtual void OnEquipped() override;
	//OnUnEquip means to put it back to the inventory
	virtual void OnUnEquipped() override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_IsEquipped();

	UPROPERTY(Replicated)
	TSubclassOf<UInventoryItemStaticData> mStaticDataClass;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipped)
	bool mIsEquipped = false;

	UPROPERTY(Replicated)
	int mValue = 0;
};
