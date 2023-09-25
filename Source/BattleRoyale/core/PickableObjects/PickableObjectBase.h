// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPickupObject.h"
#include "PickupObjectTypes.h"
#include "GameFramework/Actor.h"
#include "PickableObjectBase.generated.h"

class UInventoryItemStaticData;
class UInventoryItemInstance;

UCLASS(Abstract)
class BATTLEROYALE_API APickableObjectBase : public AActor, public IPickupObject
{
	GENERATED_BODY()

	/** Weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "PickableObject")
	USkeletalMeshComponent* Mesh;
	/** Collision*/
	UPROPERTY(EditDefaultsOnly, Category = "PickableObject")
	class USphereComponent* AreaSphere;

	UPROPERTY(ReplicatedUsing=OnRep_State, VisibleAnywhere, Category = "PickableObject")
	EPickupObjectState State = EPickupObjectState::Initial;

	UPROPERTY(EditDefaultsOnly, Category="PickableObject")
	TSubclassOf<UInventoryItemStaticData> InventoryItemStaticData;

	UPROPERTY(Replicated, EditDefaultsOnly, Category="PickableObject")
	int Value = 0;
	
public:	
	// Sets default values for this actor's properties
	APickableObjectBase();

	virtual TSubclassOf<UInventoryItemStaticData> GetInventoryItemStaticData() const override { return InventoryItemStaticData; }
	
	UFUNCTION(BlueprintCallable, Category="PickableObject")
	virtual USkeletalMeshComponent* GetMesh() const { return Mesh; }

	virtual int GetValue() const override { return Value; }
	virtual void SetValue(int value) override;
	virtual void SetCharacterOwner(ACharacterBase* character) override;
	
	virtual void OnEquipped() override;
	virtual void OnUnEquipped() override;
	virtual void OnDropped() override;
	
	virtual bool IsEquipped() const override { return State == EPickupObjectState::Equipped; }
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName) override;
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) override;

	virtual void DoEquipped() {};
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void OnSphereOverlapServer(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	UFUNCTION()
	void OnSphereEndOverlapServer(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
private:
	void ChangeState(EPickupObjectState state);
	virtual EPickupObjectState GetState() const override { return State; }
	void EnableDetectionArea() const;
	void DisableDetectionArea() const;
	void SetEnableMeshPhysicsAndCollision(bool enable) const;
	bool CancelPickupIndicator(AActor* OtherActor) const;
	UFUNCTION()
	void OnRep_State();
};
