#pragma once

#include "IWeaponHUD.generated.h"

UINTERFACE(Blueprintable)
class BATTLEROYALE_API UWeaponHUD : public UInterface
{
	GENERATED_BODY()
};

class IWeaponHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon HUD")
	void OnEquippedWeapon(UTexture2D* crosshair);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon HUD")
	void OnUnEquippedWeapon();
};