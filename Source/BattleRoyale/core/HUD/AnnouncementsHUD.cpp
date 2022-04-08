// Fill out your copyright notice in the Description page of Project Settings.

#include "AnnouncementsHUD.h"
#include "IAnnouncementHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Kismet/GameplayStatics.h"

const FString ANNOUNCEMENTS_HUD_NAME("AnnouncementsHUD");

AAnnouncementsHUD::AAnnouncementsHUD()
{

}

void AAnnouncementsHUD::Initialize(
	int hudIndex, 
	APlayerController* playerController, 
	TArray<TSubclassOf<UUserWidget>> announcementsHUDWidgetClasses)
{
	mHUDWidget = utils::UtilsLibrary::CreateHUDFromClass<UUserWidget>(
		hudIndex, 
		ANNOUNCEMENTS_HUD_NAME,
		playerController,
		announcementsHUDWidgetClasses[0]);

	BindToDelegate();
}

void AAnnouncementsHUD::BindToDelegate()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (gameInstance)
	{
		const auto eventDispatcher = gameInstance->GetEventDispatcher();

		eventDispatcher->OnAnnounceNewDeath.AddDynamic(this, &AAnnouncementsHUD::OnAnnounceNewDeath);
		eventDispatcher->OnAnnouncePlayerWon.AddDynamic(this, &AAnnouncementsHUD::OnAnnouncePlayerWon);
	}
}

void AAnnouncementsHUD::OnAnnounceNewDeath(const FString& killerName, const FString& victimName)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UAnnouncementHUD::StaticClass()))
	{
		IAnnouncementHUD::Execute_OnAnnounceNewDeath(mHUDWidget, killerName, victimName);
	}
}


void AAnnouncementsHUD::OnAnnouncePlayerWon()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UAnnouncementHUD::StaticClass()))
	{
		IAnnouncementHUD::Execute_OnAnnouncePlayerWon(mHUDWidget);
	}
}
