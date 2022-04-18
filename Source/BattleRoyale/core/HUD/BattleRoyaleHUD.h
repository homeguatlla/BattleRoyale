// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once 

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "MenuHUD.h"
#include "CharacterHUD.h"
#include "AnnouncementsHUD.h"
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
	
	UPROPERTY()
	UUserWidget* mCharacterHUDWidget;
	
	UPROPERTY()
	UUserWidget* mAnnouncementsHUDWidget;
	
	UPROPERTY()
	UUserWidget* mMenuHUDWidget;

protected:
	virtual void BeginPlay() override;

private:
	template<class HUDClass>
	void CreateHUD(HUDClass* instance, TArray<TSubclassOf<UUserWidget>> widgetClasses);

	UPROPERTY()
	ACharacterHUD* mCharacterHUD = nullptr;
	UPROPERTY()
	AAnnouncementsHUD* mAnnouncementsHUD = nullptr;
	UPROPERTY()
	AMenuHUD* mMenuHUD = nullptr;
};

template<class HUDClass>
void ABattleRoyaleHUD::CreateHUD(HUDClass* instance, TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	FActorSpawnParameters spawnInfo;
	spawnInfo.Owner = this;
	//spawnInfo.Instigator = this;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	instance = GetWorld()->SpawnActor<HUDClass>(
		HUDClass::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		spawnInfo);
	instance->Initialize(0, GetOwningPlayerController(), widgetClasses);
}
