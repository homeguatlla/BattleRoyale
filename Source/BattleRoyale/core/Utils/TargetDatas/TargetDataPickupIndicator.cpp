
#include "TargetDataPickupIndicator.h"

bool FTargetDataPickupIndicator::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	Ar << Location;

	bOutSuccess = true;
	return true;
}

FString FTargetDataPickupIndicator::ToString() const
{
	return FString::Printf(TEXT("Pickup Location %s"), *Location.ToString());
}