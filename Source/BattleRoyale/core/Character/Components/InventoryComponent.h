// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IInventoryComponent.h"
#include "BattleRoyale/core/PickableObjects/Ammo/AmmoTypes.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArray.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryBag.h"
#include "Components/ActorComponent.h"
#include "InventoryComponent.generated.h"

class IWeapon;
class UPickupObject;
class UMyReplicatedObject;

//TODO create a delegate to know when something is equipped, unequipped or dropped
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedPickableObject, TScriptInterface<IPickupObject> object);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnEquippedItemWeapon, TScriptInterface<IWeapon> weapon, int32 totalAmmo);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnPickedUpAmmo, EAmmoType type, int32 totalAmmo);

DECLARE_MULTICAST_DELEGATE(FOnDroppedPickableObject);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BATTLEROYALE_API UInventoryComponent : public UActorComponent, public IIInventoryComponent
{
	GENERATED_BODY()

public:
	FOnEquippedPickableObject OnEquippedPickableObjectDelegate;
	FOnEquippedItemWeapon OnEquippedWeaponDelegate;
	FOnPickedUpAmmo OnPickedUpAmmoDelegate;
	FOnDroppedPickableObject OnDroppedPickableObjectDelegate;
	
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
	virtual bool HasItemEquipped() const override { return mEquippedObject != nullptr; }

	virtual bool HasItemOfType(TSubclassOf<UInventoryItemStaticData> itemStaticDataClassToFind) const override;
	virtual bool HasAmmoOfType(EAmmoType ammoType) const override;
	virtual bool HasLifeKid() const override;

	virtual int GetTotalAmmoOfType(EAmmoType ammoType) const override;
	virtual int RemoveEnoughAmmo(EAmmoType ammoType, int ammoNeeded) override;
	virtual void PerformActionForEachInventoryItem(const std::function<bool (UInventoryArrayItem* inventoryItem)>& callback) const override;
	
	void OnInventoryKeyPressed();

protected:
	// Called when the game starts
	virtual void InitializeComponent() override;
	virtual bool ReplicateSubobjects(UActorChannel* Channel, FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void BeginDestroy() override;
	
private:
	virtual bool EquipItem(TScriptInterface<IPickupObject> pickableObject) override;
	TScriptInterface<IIInventoryItemInstance> GetAmmoItemOfType(EAmmoType ammoType) const;
	void NotifyEquippedWeapon(TScriptInterface<IPickupObject> pickableObject) const;
	void NotifyIfPickedUpObjectIsAmmo(TScriptInterface<IPickupObject> pickableObject) const;
	bool IsAWeapon(TScriptInterface<IPickupObject> pickableObject) const;
	int GetTotalWeapons() const;
	
	UFUNCTION()
	void OnRep_EquippedObject() const;
	UFUNCTION()
	void OnRep_InventoryBag() const;

	UFUNCTION(Client, Reliable)
	void ClientNotifyPickedUpObject(APickableObjectBase* pickedUpObject) const;
	
	//void UnEquipItem();
	//void DropItem();

	UPROPERTY(EditDefaultsOnly)
	int MaxInventoryItems = 7;

	UPROPERTY(EditDefaultsOnly)
	int MaxInventoryWeapons = 2;
	
	UPROPERTY(ReplicatedUsing=OnRep_InventoryBag)
	UInventoryBag* mInventoryBag = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TArray<TSubclassOf<UInventoryItemStaticData>> DefaultItems;

	UPROPERTY(ReplicatedUsing=OnRep_EquippedObject)
	TScriptInterface<IPickupObject> mEquippedObject = nullptr;
	
	bool mIsInventoryShown = false;
};
