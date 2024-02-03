// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/PickableObjects/IPickupObject.h"
#include "PickableObjectMock.generated.h"

UCLASS()
class BATTLEROYALETESTS_API APickableObjectMock : public AActor, public IPickupObject
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	APickableObjectMock();
	void Initialize(const FName& name, int value, TSubclassOf<UInventoryItemStaticData> staticData);
	
	virtual EPickupObjectState GetState() const override { return EPickupObjectState::Initial;}
	virtual int GetValue() const override { return mValue; }
	virtual FVector GetLocation() const override { return FVector::Zero(); }
	virtual FName GetPickableName() const override { return mName;}
	virtual void SetValue(int value) override {}
	virtual void SetCharacterOwner(ACharacterBase* character) override {}
	virtual void OnEquipped() override{}
	virtual void OnUnEquipped() override{}
	virtual void OnDropped() override{}
	virtual bool IsEquipped() const override{return false;}
	virtual bool CanBeEquipped() const override { return true; }
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent,
		const FAttachmentTransformRules& attachmentRules, const FName& socketName) override{ return true;}
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) override{}
	virtual TSubclassOf<UInventoryItemStaticData> GetInventoryItemStaticData() const { return mInventoryItemStaticData; }

private:

	FName mName;
	int mValue;
	TSubclassOf<UInventoryItemStaticData> mInventoryItemStaticData;
};