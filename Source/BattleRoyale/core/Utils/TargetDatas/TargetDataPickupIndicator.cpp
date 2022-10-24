
#include "TargetDataPickupIndicator.h"

bool FTargetDataPickupIndicator::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << mLocation;
	Ar << mPickableObject;

	bOutSuccess = true;
	return true;
}

FString FTargetDataPickupIndicator::ToString() const
{
	return FString::Printf(TEXT("Pickup Location %s, Object %s"), *mLocation.ToString(), *mPickableObject->GetName());
}