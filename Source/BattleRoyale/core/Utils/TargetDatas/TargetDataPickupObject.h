
#pragma once
#include "CoreMinimal.h"
#include "Abilities/GameplayAbilityTargetTypes.h"
#include "BattleRoyale/core/General/PickupObjectBase.h"
#include "TargetDataPickupObject.generated.h"

USTRUCT(BlueprintType)
struct BATTLEROYALE_API FTargetDataPickupObject : public FGameplayAbilityTargetData
{
	GENERATED_USTRUCT_BODY()
	
	FTargetDataPickupObject() = default;
	
	FTargetDataPickupObject(const FVector& location, APickupObjectBase* pickableObject)
	{
		mLocation = location;		
		mPickableObject = pickableObject;
	}

	FVector GetLocation() const { return mLocation; }
	TScriptInterface<IPickupObject> GetPickableObject() const { return mPickableObject; }
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FTargetDataPickupObject::StaticStruct();
	}

	virtual FString ToString() const override;

	bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess);
	
	UPROPERTY()
	FVector mLocation;
	UPROPERTY()
	APickupObjectBase* mPickableObject;
};

template<>
struct TStructOpsTypeTraits<FTargetDataPickupObject> : public TStructOpsTypeTraitsBase2<FTargetDataPickupObject>
{
	enum
	{
		WithNetSerializer = true	// For now this is REQUIRED for FGameplayAbilityTargetDataHandle net serialization to work
    };
};