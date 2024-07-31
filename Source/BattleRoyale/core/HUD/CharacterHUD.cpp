// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterHUD.h"

#include "IGameHUD.h"
#include "IHealthHUD.h"
#include "IPlayerHUD.h"
#include "ISkillsHUD.h"
#include "IWeaponHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Kismet/GameplayStatics.h"

const FString CHARACTER_HUD_NAME("CharacterHUD");

ACharacterHUD::ACharacterHUD() : mHUDWidget(nullptr)
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
		eventDispatcher->OnRefreshTotalAmmo.AddDynamic(this, &ThisClass::OnRefreshTotalAmmo);
		
		eventDispatcher->OnPlayerDead.AddDynamic(this, &ThisClass::OnPlayerDead);
		eventDispatcher->OnRefreshNumKills.AddDynamic(this, &ThisClass::OnRefreshNumKills);

		eventDispatcher->OnGameStarted.AddDynamic(this, &ThisClass::OnGameStarted);

		eventDispatcher->OnInitializeSkills.AddDynamic(this, &ThisClass::OnInitializeSkills);
		eventDispatcher->OnStartSkillCooldown.AddDynamic(this, &ThisClass::OnStartSkillCoolDown);
		eventDispatcher->OnStopSkillCooldown.AddDynamic(this, &ThisClass::OnStopSkillCoolDown);
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

void ACharacterHUD::OnRefreshCrosshair(float spread, AActor* targetActor, bool isAiming, bool isMagazineAlmostEmpty)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UPlayerHUD::StaticClass()))
	{
		IPlayerHUD::Execute_OnRefreshCrosshair(mHUDWidget, spread, targetActor, isAiming, isMagazineAlmostEmpty);
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

void ACharacterHUD::OnRefreshTotalAmmo(int32 totalAmmo)
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UPlayerHUD::StaticClass()))
	{
		IPlayerHUD::Execute_OnRefreshTotalAmmo(mHUDWidget, totalAmmo);
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

void ACharacterHUD::OnInitializeSkills(const TArray<FSkillData>& skills)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(USkillsHUD::StaticClass()))
	{
		ISkillsHUD::Execute_OnInitializeSkills(mHUDWidget, skills);
	}
}

void ACharacterHUD::OnStartSkillCoolDown(const FGameplayTag& skillTag)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(USkillsHUD::StaticClass()))
	{
		ISkillsHUD::Execute_OnStartSkillCooldown(mHUDWidget, skillTag);
	}
}

void ACharacterHUD::OnStopSkillCoolDown(const FGameplayTag& skillTag)
{
	if(mHUDWidget->GetClass()->ImplementsInterface(USkillsHUD::StaticClass()))
	{
		ISkillsHUD::Execute_OnStopSkillCooldown(mHUDWidget, skillTag);
	}
}

