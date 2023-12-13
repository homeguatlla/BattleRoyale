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
	virtual void Initialize(TSubclassOf<UInventoryItemStaticData> itemStaticClass, int value1, int value2) override;
	virtual const UInventoryItemStaticData* GetStaticData() const override;
	virtual const TSubclassOf<UInventoryItemStaticData> GetStaticDataClass() const override { return mStaticDataClass; }
	virtual int GetValue1() const override { return mValue1; }
	virtual void UpdateValue1(int newValue) override;
	virtual int GetValue2() const override { return mValue2; }
	virtual void UpdateValue2(int newValue) override;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(Replicated)
	TSubclassOf<UInventoryItemStaticData> mStaticDataClass;
	
	UPROPERTY(Replicated)
	int mValue1 = 0;
	UPROPERTY(Replicated)
	int mValue2 = 0;
};
