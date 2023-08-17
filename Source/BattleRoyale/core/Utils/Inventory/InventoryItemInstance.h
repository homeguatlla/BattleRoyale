// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "InventoryItemInstance.generated.h"

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
	void Initialize(TSubclassOf<UInventoryItemStaticData> staticDataClass);
	const UInventoryItemStaticData* GetStaticData() const;

private:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnRep_IsEquipped();

	//OnEquip means to put it on hand
	void OnEquipped();
	//OnUnEquip means to put it back to the inventory
	void OnUnEquipped();
	
	UPROPERTY(Replicated)
	TSubclassOf<UInventoryItemStaticData> mStaticDataClass;

	UPROPERTY(ReplicatedUsing=OnRep_IsEquipped)
	bool mIsEquipped = false;
};
