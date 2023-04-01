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
		eventDispatcher->OnGameStarted.AddDynamic(this, &ThisClass::OnGameStarted);
		eventDispatcher->OnGameOver.AddDynamic(this, &ThisClass::OnGameOver);
		
		eventDispatcher->OnRefreshCountDown.AddDynamic(this, &ThisClass::OnRefreshCountDown);
		eventDispatcher->OnFinishCountDown.AddDynamic(this, &ThisClass::OnFinishCountDown);
		eventDispatcher->OnSetVisibilityCountDown.AddDynamic(this, &ThisClass::OnSetVisibilityCountDown);

		eventDispatcher->OnShowStatsScreen.AddDynamic(this, &ThisClass::OnShowStatsScreen);
		eventDispatcher->OnHideStatsScreen.AddDynamic(this, &ThisClass::OnHideStatsScreen);
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

void AMenuHUD::OnSetVisibilityCountDown(bool visible)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UCounterDownMenuHUD::StaticClass()))
	{
		ICounterDownMenuHUD::Execute_OnSetVisibilityCountDown(mHUDWidget, visible);
	}
}

void AMenuHUD::OnShowStatsScreen(const FPlayerStatsData& playerStatsData)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UStatsMenuHUD::StaticClass()))
	{
		IStatsMenuHUD::Execute_OnShowStatsScreen(mHUDWidget, playerStatsData);
	}
}

void AMenuHUD::OnHideStatsScreen()
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UStatsMenuHUD::StaticClass()))
	{
		IStatsMenuHUD::Execute_OnHideStatsScreen(mHUDWidget);
	}
}
