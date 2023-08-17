// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPickupObject.h"
#include "PickupObjectTypes.h"
#include "GameFramework/Actor.h"
#include "PickupObjectBase.generated.h"

UCLASS()
class BATTLEROYALE_API APickupObjectBase : public AActor, public IPickupObject
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
	
public:	
	// Sets default values for this actor's properties
	APickupObjectBase();
	
	virtual FVector GetPickupWidgetLocation() const override;
	
	UFUNCTION(BlueprintCallable, Category="PickableObject")
	virtual USkeletalMeshComponent* GetMesh() const { return Mesh; }

	virtual void OnEquipped() override;
	virtual void OnDropped() override;
	
	virtual bool IsEquipped() const override { return State == EPickupObjectState::Equipped; }
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules, const FName& socketName) override;
	virtual void DetachFromComponent(const FDetachmentTransformRules& rules) override;

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
	UFUNCTION()
	void OnRep_State();
};
