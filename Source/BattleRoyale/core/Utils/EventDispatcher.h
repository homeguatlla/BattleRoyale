#pragma once
#include "CoreMinimal.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"
#include "EventDispatcher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeapon, TScriptInterface<IIWeapon>, weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquippedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRefreshHealth, float, health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnnounceNewDeath);

UCLASS(Blueprintable)
class BATTLEROYALE_API UEventDispatcher : public UObject
{
	GENERATED_BODY()

public:
	UEventDispatcher() = default;

	UPROPERTY(BlueprintAssignable, Category="Character Health")
	FRefreshHealth OnRefreshHealth;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnEquippedWeapon OnEquippedWeapon;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnUnEquippedWeapon OnUnEquippedWeapon;

	UPROPERTY(BlueprintAssignable, Category="Player Dead")
	FOnPlayerDead OnPlayerDead;

	UPROPERTY(BlueprintAssignable, Category="Announcement New Death")
	FOnAnnounceNewDeath OnAnnounceNewDeath;
};
