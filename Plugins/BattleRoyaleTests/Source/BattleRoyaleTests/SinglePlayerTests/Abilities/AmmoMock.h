// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "AmmoMock.generated.h"

class UInventoryItemStaticDataMock;

UCLASS()
class BATTLEROYALETESTS_API AAmmoMock : public AActor, public IPickupObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AAmmoMock();

	void Initialize(const FName& itemStaticDataName, EPickupObjectState state, int value);
	
	virtual TSubclassOf<UInventoryItemStaticData> GetInventoryItemStaticData() const override { return mItemStaticData; }
	virtual EPickupObjectState GetState() const override { return mState;}
	virtual int GetValue() const override { return mValue; }
	virtual FVector GetLocation() const override { return FVector::Zero(); }
	virtual FName GetPickableName() const override { return ""; }
	virtual void SetValue(int value) override { mValue = value; }
	virtual void SetCharacterOwner(ACharacterBase* character) override {}
	virtual void OnEquipped() override {}
	virtual void OnUnEquipped() override {}
	virtual void OnDropped() override {}
	virtual bool IsEquipped() const override { return mState == EPickupObjectState::Equipped; }
	virtual bool CanBeEquipped() const override { return true; }
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent,
		const FAttachmentTransformRules& attachmentRules, const FName& socketName) override { return true; }
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) override {}

private:
	UPROPERTY()
	TSubclassOf<UInventoryItemStaticData>  mItemStaticData = nullptr;
	EPickupObjectState mState = EPickupObjectState::Initial;
	int mValue = 0;
};
