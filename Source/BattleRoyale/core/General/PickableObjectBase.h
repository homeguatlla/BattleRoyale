// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IPickableObject.h"
#include "GameFramework/Actor.h"
#include "PickableObjectBase.generated.h"

UCLASS()
class BATTLEROYALE_API APickableObjectBase : public AActor, public IPickableObject
{
	GENERATED_BODY()

	/** Weapon mesh */
	UPROPERTY(EditDefaultsOnly, Category = "PickableObject")
	USkeletalMeshComponent* Mesh;
	/** Collision*/
	UPROPERTY(EditDefaultsOnly, Category = "PickableObject")
	class USphereComponent* AreaSphere;
	
public:	
	// Sets default values for this actor's properties
	APickableObjectBase();
	
	virtual FVector GetPickupWidgetLocation(const FBoxSphereBounds& bounds) const override;
	virtual USkeletalMeshComponent* GetMesh() const { return Mesh; }
	virtual bool AttachToComponent(USkeletalMeshComponent* meshComponent, const FAttachmentTransformRules& attachmentRules,
							   const FName& socketName) override;
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
};