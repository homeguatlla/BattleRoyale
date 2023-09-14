#pragma once

UENUM(BlueprintType)
enum class EAmmoType : uint8
{
	LightAmmo UMETA(DisplayName = "Light Ammo"),
	HeavyAmmo UMETA(DisplayName = "Heavy Ammo"),

	Max UMETA(DisplayName = "MAX Ammo Type")
};