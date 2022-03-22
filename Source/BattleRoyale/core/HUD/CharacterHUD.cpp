// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterHUD.h"

#include "IHealthHUD.h"
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

		eventDispatcher->OnEquippedWeapon.AddDynamic(this, &ACharacterHUD::OnEquippedWeapon);
		eventDispatcher->OnUnEquippedWeapon.AddDynamic(this, &ACharacterHUD::OnUnEquippedWeapon);
		eventDispatcher->OnRefreshHealth.AddDynamic(this, &ACharacterHUD::OnRefreshHealthReceived);
	}
}

void ACharacterHUD::OnEquippedWeapon(TScriptInterface<IIWeapon> weapon)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UWeaponHUD::StaticClass()))
	{
		IWeaponHUD::Execute_OnEquippedWeapon(mHUDWidget, weapon->GetCrossHairTexture());
	}
}

void ACharacterHUD::OnUnEquippedWeapon()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UWeaponHUD::StaticClass()))
	{
		IWeaponHUD::Execute_OnUnEquippedWeapon(mHUDWidget);
	}
}

void ACharacterHUD::OnRefreshHealthReceived(float health)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UHealthHUD::StaticClass()))
	{
		IHealthHUD::Execute_OnRefreshHealth(mHUDWidget, health);
	}
}