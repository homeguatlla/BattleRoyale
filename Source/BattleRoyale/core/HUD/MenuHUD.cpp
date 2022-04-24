// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuHUD.h"

#include "ICounterDownMenuHUD.h"
#include "IGameHUD.h"
#include "IStatsMenuHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Kismet/GameplayStatics.h"

const FString MENU_HUD_NAME("MenuHUD");

AMenuHUD::AMenuHUD()
{

}

void AMenuHUD::Initialize(
	int hudIndex, 
	APlayerController* playerController, 
	TArray<TSubclassOf<UUserWidget>> widgetsClasses)
{
	mHUDWidget = utils::UtilsLibrary::CreateHUDFromClass<UUserWidget>(
		hudIndex, 
		MENU_HUD_NAME,
		playerController,
		widgetsClasses[0]);

	BindToDelegate();
}

void AMenuHUD::BindToDelegate()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (gameInstance)
	{
		const auto eventDispatcher = gameInstance->GetEventDispatcher();
		eventDispatcher->OnGameStarted.AddDynamic(this, &AMenuHUD::OnGameStarted);
		eventDispatcher->OnGameOver.AddDynamic(this, &AMenuHUD::OnGameOver);
		
		eventDispatcher->OnRefreshCountDown.AddDynamic(this, &AMenuHUD::OnRefreshCountDown);
		eventDispatcher->OnFinishCountDown.AddDynamic(this, &AMenuHUD::OnFinishCountDown);

		eventDispatcher->OnShowStatsScreen.AddDynamic(this, &AMenuHUD::OnShowStatsScreen);
	}
}

void AMenuHUD::OnGameStarted()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UGameHUD::StaticClass()))
	{
		IGameHUD::Execute_OnGameStarted(mHUDWidget);
	}
}

void AMenuHUD::OnGameOver()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UGameHUD::StaticClass()))
	{
		IGameHUD::Execute_OnGameOver(mHUDWidget);
	}
}

void AMenuHUD::OnRefreshCountDown(uint8 counter)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UCounterDownMenuHUD::StaticClass()))
	{
		ICounterDownMenuHUD::Execute_OnRefreshCountDown(mHUDWidget, counter);
	}
}

void AMenuHUD::OnFinishCountDown()
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UCounterDownMenuHUD::StaticClass()))
	{
		ICounterDownMenuHUD::Execute_OnFinishCountDown(mHUDWidget);
	}
}

void AMenuHUD::OnShowStatsScreen(const FPlayerStatsData& playerStatsData)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UStatsMenuHUD::StaticClass()))
	{
		IStatsMenuHUD::Execute_OnShowStatsScreen(mHUDWidget, playerStatsData);
	}
}
