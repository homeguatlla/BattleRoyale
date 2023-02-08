#pragma once

#include "IWeaponHUD.generated.h"

UINTERFACE(Blueprintable)
class BATTLEROYALE_API UWeaponHUD : public UInterface
{
	GENERATED_BODY()
};

class ICrosshair;

class IWeaponHUD
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon HUD")
	void OnEquippedWeapon(const TScriptInterface<ICrosshair>& crosshair);
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Weapon HUD")
	void OnUnEquippedWeapon();
};