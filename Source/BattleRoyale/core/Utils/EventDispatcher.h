#pragma once
#include "CoreMinimal.h"
#include "BattleRoyale/core/Weapons/IWeapon.h"
#include "BattleRoyale/core/Utils/Stats/PlayerStatsData.h"
#include "EventDispatcher.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquippedWeapon, TScriptInterface<IIWeapon>, weapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnUnEquippedWeapon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRefreshHealth, float, health);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRefreshNumKills, int, numKills);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameStarted);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnGameOver);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerDead);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnnounceNewDeath, const FString&, killerName, const FString&, victimName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnAnnouncePlayerWon);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAnnounceTeamsAndPlayersAlive, uint8, numTeams, uint8, numPlayers);


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFinishCountDown);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRefreshCountDown, uint8, counter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnShowStatsScreen, const FPlayerStatsData&, statsData);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnHideStatsScreen);

UCLASS(Blueprintable)
class BATTLEROYALE_API UEventDispatcher : public UObject
{
	GENERATED_BODY()

	
public:
	UEventDispatcher() = default;

	UPROPERTY(BlueprintAssignable, Category="Character")
	FRefreshHealth OnRefreshHealth;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnEquippedWeapon OnEquippedWeapon;

	UPROPERTY(BlueprintAssignable, Category="Character Weapon")
	FOnUnEquippedWeapon OnUnEquippedWeapon;

	UPROPERTY(BlueprintAssignable, Category="Character Stats")
	FRefreshNumKills OnRefreshNumKills;
	
	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnPlayerDead OnPlayerDead;
	
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnGameStarted OnGameStarted;
	
	UPROPERTY(BlueprintAssignable, Category="Game")
	FOnGameOver OnGameOver;
	
	UPROPERTY(BlueprintAssignable, Category="Announcement")
	FOnAnnounceNewDeath OnAnnounceNewDeath;

	UPROPERTY(BlueprintAssignable, Category="Player")
	FOnAnnouncePlayerWon OnAnnouncePlayerWon;

	UPROPERTY(BlueprintAssignable, Category="Announcement")
	FOnAnnounceTeamsAndPlayersAlive OnAnnounceTeamsAndPlayersAlive;
	
	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnFinishCountDown OnFinishCountDown;

	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnRefreshCountDown OnRefreshCountDown;

	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnShowStatsScreen OnShowStatsScreen;

	UPROPERTY(BlueprintAssignable, Category="Menu")
	FOnHideStatsScreen OnHideStatsScreen;
};
