// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "Components/ActorComponent.h"
#include "PickupSelectorComponent.generated.h"

class IPickupObject;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BATTLEROYALE_API UPickupSelectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "PickupIndicator")
	TSubclassOf<UGameplayEffect> InitializePickupIndicatorEffect;

	// Sets default values for this component's properties
	UPickupSelectorComponent();

	virtual void BeginPlay() override;
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void RegisterOverlapEvents();
	void SelectPickupObject();
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	UFUNCTION()
	void OnCapsuleBeginOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp,
	                           int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCapsuleEndOverlapServer(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                               UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	void NotifyOrCancelSelectedPickupObject() const;

	UFUNCTION()
	void OnRep_SelectedPickableObject();
	void CancelPickupIndicator(TScriptInterface<IICharacter> character) const;

	TArray<TScriptInterface<IPickupObject>> mPickableObjectsCollisionList;

	UPROPERTY(ReplicatedUsing=OnRep_SelectedPickableObject)
	TScriptInterface<IPickupObject> mSelectedPickableObject { nullptr };
};
