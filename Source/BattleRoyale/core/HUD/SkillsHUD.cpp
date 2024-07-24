// Fill out your copyright notice in the Description page of Project Settings.


#include "SkillsHUD.h"

#include "ISkillsHUD.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "Blueprint/UserWidget.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"

const FString SKILLS_HUD_NAME("SkillsHUD");

void ASkillsHUD::Initialize(int hudIndex, APlayerController* playerController,
	TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	mHUDWidget = utils::UtilsLibrary::CreateHUDFromClass<UUserWidget>(
		hudIndex, 
		SKILLS_HUD_NAME,
		playerController,
		widgetClasses[0]);
	
	BindToDelegate();
}

void ASkillsHUD::BindToDelegate()
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(UGameplayStatics::GetGameInstance(this->GetWorld()));
	if (gameInstance)
	{
		const auto eventDispatcher = gameInstance->GetEventDispatcher();
		eventDispatcher->OnRefreshSkillsCooldowns.AddDynamic(this, &ThisClass::OnRefreshSkillsCooldowns);
	}
}

void ASkillsHUD::OnRefreshSkillsCooldowns()
{
	if (mHUDWidget->GetClass()->ImplementsInterface(UISkillsHUD::StaticClass()))
	{
		IISkillsHUD::Execute_OnRefreshSkillsCooldowns(mHUDWidget);
	}
}
