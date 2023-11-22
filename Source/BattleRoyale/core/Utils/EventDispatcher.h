#pragma once
#include "CoreMinimal.h"
#include "BattleRoyale/core/Utils/Stats/PlayerStatsData.h"
#include "Inventory/InventoryBag.h"
#include "Delegates/DelegateCombinations.h"
#include "EventDispatcher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeapon, TScriptInterface<IWeapon>, weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquippedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnRefreshCrosshair, float, spread, AActor*, targetActor, bool, isAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRefreshHealth, float, health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRefreshNumKills, int, numKills);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRefreshAmmo, int32, ammo, int32, magazineCapacity);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRefreshTotalAmmo, int32, totalAmmo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnnounceNewDeath, const FString&, killerName, const FString&, victimName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnnouncePlayerWon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnnounceTeamsAndPlayersAlive, uint8, numTeams, uint8, numPlayers);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishCountDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshCountDown, uint8, counter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSetVisibilityCountDown, bool, visible);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowStatsScreen, const FPlayerStatsData&, statsData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideStatsScreen);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowInventoryScreen, const TScriptInterface<IIInventoryBag>, inventoryBag);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideInventoryScreen);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshInventory, const TScriptInterface<IIInventoryBag>, inventoryBag);


UCLASS(Blueprintable)
class BATTLEROYALE_API UEventDispatcher : public UObject
{
	GENERATED_BODY()

	
public:
	UEventDispatcher() = default;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FRefreshHealth OnRefreshHealth;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FRefreshTotalAmmo OnRefreshTotalAmmo;
	
	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnEquippedWeapon OnEquippedWeapon;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnUnEquippedWeapon OnUnEquippedWeapon;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnRefreshCrosshair OnRefreshCrosshair;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FRefreshAmmo OnRefreshAmmo;
	
	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnPlayerDead OnPlayerDead;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FRefreshNumKills OnRefreshNumKills;
	
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnGameStarted OnGameStarted;
	
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnGameOver OnGameOver;
	
	UPROPERTY(BlueprintAssignable, Category="Announcement")
	FOnAnnounceNewDeath OnAnnounceNewDeath;

	UPROPERTY(BlueprintAssignable, Category="Announcement")
	FOnAnnouncePlayerWon OnAnnouncePlayerWon;

	UPROPERTY(BlueprintAssignable, Category="Announcement")
	FOnAnnounceTeamsAndPlayersAlive OnAnnounceTeamsAndPlayersAlive;
	
	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnFinishCountDown OnFinishCountDown;

	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnRefreshCountDown OnRefreshCountDown;

	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnSetVisibilityCountDown OnSetVisibilityCountDown;
	
	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnShowStatsScreen OnShowStatsScreen;

	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnHideStatsScreen OnHideStatsScreen;

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnShowInventoryScreen OnShowInventoryScreen;
	
	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnHideInventoryScreen OnHideInventoryScreen;

	UPROPERTY(BlueprintAssignable, Category="Inventory")
	FOnRefreshInventory OnRefreshInventory;
};
