// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryHUD.h"

#include "IInventoryHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryBagVisual.h"
#include "Kismet/GameplayStatics.h"

const FString INVENTORY_HUD_NAME("InventoryHUD");

void AInventoryHUD::Initialize(int maxInventoryItems, int hudIndex, APlayerController* playerController,
	TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	mHUDWidget = utils::UtilsLibrary::CreateHUDFromClass<UUserWidget>(
		hudIndex, 
		INVENTORY_HUD_NAME,
		playerController,
		widgetClasses[0]);

	inventoryBagVisual = NewObject<UInventoryBagVisual>();
	inventoryBagVisual->Initialize(maxInventoryItems);
	BindToDelegate();
}

void AInventoryHUD::BindToDelegate()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (gameInstance)
	{
		const auto eventDispatcher = gameInstance->GetEventDispatcher();
		eventDispatcher->OnShowInventoryScreen.AddDynamic(this, &ThisClass::OnShowInventoryScreen);
		eventDispatcher->OnHideInventoryScreen.AddDynamic(this, &ThisClass::OnHideInventoryScreen);
		eventDispatcher->OnRefreshInventory.AddDynamic(this, &ThisClass::OnRefreshInventory);
	}
}

void AInventoryHUD::OnShowInventoryScreen(const TScriptInterface<IIInventoryBag> inventoryBag)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UIInventoryHUD::StaticClass()))
	{
		inventoryBagVisual->Refresh(inventoryBag);
		IIInventoryHUD::Execute_OnShowInventoryScreen(mHUDWidget, inventoryBagVisual);
	}
}

void AInventoryHUD::OnHideInventoryScreen()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UIInventoryHUD::StaticClass()))
	{
		IIInventoryHUD::Execute_OnHideInventoryScreen(mHUDWidget);
	}
}

void AInventoryHUD::OnRefreshInventory(const TScriptInterface<IIInventoryBag> inventoryBag)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UIInventoryHUD::StaticClass()))
	{
		inventoryBagVisual->Refresh(inventoryBag);
		IIInventoryHUD::Execute_OnRefreshInventory(mHUDWidget, inventoryBagVisual);
	}
}
