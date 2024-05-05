// Fill out your copyright notice in the Description page of Project Settings.


#include "NetworkHUD.h"

#include "IConnectivityHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Blueprint/UserWidget.h"

const FString NETWORK_HUD_NAME("NetworkHUD");

void ANetworkHUD::Initialize(int hudIndex, APlayerController* playerController,
	TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	mHUDWidget = utils::UtilsLibrary::CreateHUDFromClass<UUserWidget>(
		hudIndex, 
		NETWORK_HUD_NAME,
		playerController,
		widgetClasses[0]);
	
	BindToDelegate();
}

void ANetworkHUD::BindToDelegate()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (gameInstance)
	{
		const auto eventDispatcher = gameInstance->GetEventDispatcher();
		eventDispatcher->OnShowConnectivity.AddDynamic(this, &ThisClass::OnShowConnectivity);
	}
}

void ANetworkHUD::OnShowConnectivity(int ping)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UConnectivityHUD::StaticClass()))
	{
		IConnectivityHUD::Execute_OnShowConnectivity(mHUDWidget, ping);
	}
}
