// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterHUD.h"

#include "IGameHUD.h"
#include "IHealthHUD.h"
#include "IPlayerHUD.h"
#include "IWeaponHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Kismet/GameplayStatics.h"

const FString CHARACTER_HUD_NAME("CharacterHUD");

ACharacterHUD::ACharacterHUD()
{

}

void ACharacterHUD::Initialize(
	int hudIndex, 
	APlayerController* playerController, 
	TArray<TSubclassOf<UUserWidget>> widgetsClasses)
{
	mHUDWidget = utils::UtilsLibrary::CreateHUDFromClass<UUserWidget>(
		hudIndex, 
		CHARACTER_HUD_NAME,
		playerController,
		widgetsClasses[0]);

	BindToDelegate();
}

void ACharacterHUD::BindToDelegate()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (gameInstance)
	{
		const auto eventDispatcher = gameInstance->GetEventDispatcher();

		eventDispatcher->OnEquippedWeapon.AddDynamic(this, &ThisClass::OnEquippedWeapon);

		eventDispatcher->OnUnEquippedWeapon.AddDynamic(this, &ThisClass::OnUnEquippedWeapon);
		eventDispatcher->OnRefreshCrosshair.AddDynamic(this, &ThisClass::OnRefreshCrosshair);
		eventDispatcher->OnRefreshHealth.AddDynamic(this, &ThisClass::OnRefreshHealthReceived);
		eventDispatcher->OnRefreshAmmo.AddDynamic(this, &ThisClass::OnRefreshAmmo);
		
		eventDispatcher->OnPlayerDead.AddDynamic(this, &ThisClass::OnPlayerDead);
		eventDispatcher->OnRefreshNumKills.AddDynamic(this, &ThisClass::OnRefreshNumKills);

		eventDispatcher->OnGameStarted.AddDynamic(this, &ThisClass::OnGameStarted);
	}
}

void ACharacterHUD::OnEquippedWeapon(TScriptInterface<IWeapon> weapon)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UWeaponHUD::StaticClass()))
	{
		IWeaponHUD::Execute_OnEquippedWeapon(mHUDWidget, weapon->GetCrosshairWidget());
	}
}

void ACharacterHUD::OnUnEquippedWeapon()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UWeaponHUD::StaticClass()))
	{
		IWeaponHUD::Execute_OnUnEquippedWeapon(mHUDWidget);
	}
}

void ACharacterHUD::OnRefreshCrosshair(float spread, AActor* targetActor, bool isAiming)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UPlayerHUD::StaticClass()))
	{
		IPlayerHUD::Execute_OnRefreshCrosshair(mHUDWidget, spread, targetActor, isAiming);
	}
}

void ACharacterHUD::OnRefreshHealthReceived(float health)
{
	//TODO esto se puede poner,
	//if(mHUDWidget->Implements<UHealthHUD>()), esto no funciona. Googlear a ver porque.
	
	
	if (mHUDWidget->GetClass()->ImplementsInterface(UHealthHUD::StaticClass()))
	{
		IHealthHUD::Execute_OnRefreshHealth(mHUDWidget, health);
	}
}

void ACharacterHUD::OnRefreshAmmo(int32 ammo, int32 magazineCapacity)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UWeaponHUD::StaticClass()))
	{
		IWeaponHUD::Execute_OnRefreshAmmo(mHUDWidget, ammo, magazineCapacity);
	}
}

void ACharacterHUD::OnPlayerDead()
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UPlayerHUD::StaticClass()))
	{
		IPlayerHUD::Execute_OnPlayerDead(mHUDWidget);
	}
}

void ACharacterHUD::OnRefreshNumKills(int numKills)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UPlayerHUD::StaticClass()))
	{
		IPlayerHUD::Execute_OnRefreshNumKills(mHUDWidget, numKills);
	}
}

void ACharacterHUD::OnGameStarted()
{
	if(mHUDWidget->GetClass()->ImplementsInterface(UGameHUD::StaticClass()))
	{
		IGameHUD::Execute_OnGameStarted(mHUDWidget);
	}
}

