#pragma once

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	TurnLeft UMETA(DisplayName = "Turning Left"),
	TurnRight UMETA(DisplayName = "Turning Right"),
	NotTurning UMETA(DisplayName = "Not Turning"),

	Max UMETA(DisplayName = "MAX Turn in place")
};