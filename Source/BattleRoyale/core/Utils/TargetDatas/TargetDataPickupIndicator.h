
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "BattleRoyale/core/General/PickableObjectBase.h"
#include "TargetDataPickupIndicator.generated.h"

USTRUCT(BlueprintType)
struct BATTLEROYALE_API FTargetDataPickupIndicator : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()
	
	FTargetDataPickupIndicator() = default;
	
	FTargetDataPickupIndicator(const FVector& location, APickableObjectBase* pickableObject)
	{
		mLocation = location;		
		mPickableObject = pickableObject;
	}

	FVector GetLocation() const { return mLocation; }
	TScriptInterface<IPickableObject> GetPickableObject() const { return mPickableObject; }
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTargetDataPickupIndicator::StaticStruct();
	}

	virtual FString ToString() const override;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	
	UPROPERTY()
	FVector mLocation;
	UPROPERTY()
	APickableObjectBase* mPickableObject;
};

template<>
struct TStructOpsTypeTraits<FTargetDataPickupIndicator> : public TStructOpsTypeTraitsBase2<FTargetDataPickupIndicator>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
    };
};