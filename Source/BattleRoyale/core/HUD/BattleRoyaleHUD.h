// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MenuHUD.h"
#include "CharacterHUD.h"
#include "AnnouncementsHUD.h"
#include "InventoryHUD.h"
#include "BattleRoyaleHUD.generated.h"


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

	UPROPERTY(EditAnywhere, Category= "Inventory HUD")
	int MaxInventoryItems = 7;
	
	UPROPERTY()
	UUserWidget* mCharacterHUDWidget;
	
	UPROPERTY()
	UUserWidget* mAnnouncementsHUDWidget;
	
	UPROPERTY()
	UUserWidget* mMenuHUDWidget;

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
