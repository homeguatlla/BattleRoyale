// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MenuHUD.h"
#include "CharacterHUD.h"
#include "AnnouncementsHUD.h"
#include "InventoryHUD.h"
#include "NetworkHUD.h"
#include "BattleRoyaleHUD.generated.h"

class UVisorPrototype;


UCLASS()
class BATTLEROYALE_API ABattleRoyaleHUD : public AHUD
{
	GENERATED_BODY()

public:
	
	/** Blueprint class which displays the ability */
	UPROPERTY(EditAnywhere, Category = "Character HUDs")
	TArray<TSubclassOf<UUserWidget>> CharacterHUDWidgetClasses;

	UPROPERTY(EditAnywhere, Category = "Announcements HUDs")
	TArray<TSubclassOf<UUserWidget>> AnnouncementsHUDWidgetClasses;

	UPROPERTY(EditAnywhere, Category = "Menu HUDs")
	TArray<TSubclassOf<UUserWidget>> MenuHUDWidgetClasses;

	UPROPERTY(EditAnywhere, Category = "Inventory HUDs")
	TArray<TSubclassOf<UUserWidget>> InventoryHUDWidgetClasses;

	UPROPERTY(EditAnywhere, Category = "Skills HUDs")
	TArray<TSubclassOf<UUserWidget>> SkillsHUDWidgetClasses;
	
	UPROPERTY(EditAnywhere, Category = "Network HUDs")
	TArray<TSubclassOf<UUserWidget>> NetworkHUDWidgetClasses;

	UPROPERTY(EditAnywhere, Category= "Inventory HUD")
	int MaxInventoryItems = 7;
	
	UPROPERTY()
	UUserWidget* mCharacterHUDWidget;
	
	UPROPERTY()
	UUserWidget* mAnnouncementsHUDWidget;
	
	UPROPERTY()
	UUserWidget* mMenuHUDWidget;

	UPROPERTY()
	UUserWidget* mNetworkWidget;

	// Visor prototype console commands (see UVisorPrototype)
	UFUNCTION(Exec)
	void VisorPrototype(int32 mode);
	UFUNCTION(Exec)
	void VisorTune(float arcAngle, float distance);
	UFUNCTION(Exec)
	void VisorWarp(float curvature, float scanlines, float aberration);
	UFUNCTION(Exec)
	void VisorHealth(float health);

protected:
	virtual void BeginPlay() override;

private:
	template<class THUDClass>
	THUDClass* CreateHUD(TArray<TSubclassOf<UUserWidget>> widgetClasses);
	template<class THUDClass>
	THUDClass* CreateHUD(int param, TArray<TSubclassOf<UUserWidget>> widgetClasses);
	
	UPROPERTY()
	ACharacterHUD* mCharacterHUD = nullptr;
	UPROPERTY()
	AAnnouncementsHUD* mAnnouncementsHUD = nullptr;
	UPROPERTY()
	AMenuHUD* mMenuHUD = nullptr;

	UPROPERTY()
	AInventoryHUD* mInventoryHUD = nullptr;

	UPROPERTY()
	ANetworkHUD* mNetworkHUD = nullptr;

	UPROPERTY()
	UVisorPrototype* mVisorPrototype = nullptr;
	UVisorPrototype* GetVisorPrototype();
};

template<class THUDClass>
THUDClass* ABattleRoyaleHUD::CreateHUD(TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	FActorSpawnParameters spawnInfo;
	spawnInfo.Owner = this;
	//spawnInfo.Instigator = this;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const auto instance = GetWorld()->SpawnActor<THUDClass>(
		THUDClass::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		spawnInfo);
	instance->Initialize(0, GetOwningPlayerController(), widgetClasses);
	return instance;
}

template<class THUDClass>
THUDClass* ABattleRoyaleHUD::CreateHUD(int param, TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	FActorSpawnParameters spawnInfo;
	spawnInfo.Owner = this;
	//spawnInfo.Instigator = this;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	const auto instance = GetWorld()->SpawnActor<THUDClass>(
		THUDClass::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		spawnInfo);
	instance->Initialize(param, 0, GetOwningPlayerController(), widgetClasses);
	return instance;
}
