// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInventoryComponent.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArray.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class UPickupObject;
//TODO create a delegate to know when something is equipped, unequipped or dropped
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedPickableObject, TScriptInterface<IPickupObject> object);
DECLARE_MULTICAST_DELEGATE(FOnDroppedPickableObject);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BATTLEROYALE_API UInventoryComponent : public UActorComponent, public IIInventoryComponent
{
	GENERATED_BODY()

public:
	FOnEquippedPickableObject OnEquippedPickableObjectDelegate;
	FOnDroppedPickableObject OnDroppedPickableObject;
	
	// Sets default values for this component's properties
	UInventoryComponent();

	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
							   FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual bool PickupObjectServer(TScriptInterface<IPickupObject> pickableObject) override;
	UFUNCTION(BlueprintCallable)
	virtual bool DropObjectServer() override;
	virtual TScriptInterface<IPickupObject> GetEquippedItem() const override;
	virtual bool HasItemEquipped() const override { return mEquippedItem != nullptr; }
	
	virtual void SetPickableObject(TScriptInterface<IPickupObject> object) override { mPickupObject = object; }
	virtual TScriptInterface<IPickupObject> GetPickableObject() const override { return mPickupObject; }
	void OnInventoryKeyPressed();

protected:
	// Called when the game starts
	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	virtual bool EquipItem(TScriptInterface<IPickupObject> pickableObject) override;
	//void UnEquipItem();
	//void DropItem();
	
	UPROPERTY(Replicated)
	FInventoryArray mInventoryArray;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UInventoryItemStaticData>> DefaultItems;

	UPROPERTY(Replicated)
	TScriptInterface<IPickupObject> mEquippedItem = nullptr;

	TScriptInterface<IPickupObject> mPickupObject;

	bool mIsInventoryShown = false;
};
