// Copyright Epic Games, Inc. All Rights Reserved.

#include "BattleRoyaleHUD.h"

#include "AutomationBlueprintFunctionLibrary.h"
#include "Engine/Canvas.h"
#include "CharacterHUD.h"
#include "Logging/LogMacros.h"
#include "UObject/ConstructorHelpers.h"


void ABattleRoyaleHUD::BeginPlay()
{
	Super::BeginPlay();

	if(CharacterHUDWidgetClasses.Num() <= 0)
	{
		//if(!UAutomationBlueprintFunctionLibrary::AreAutomatedTestsRunning()) doesn't work
		{
			UE_LOG(LogTemp, Warning, TEXT("ABattleRoyaleHUD::BeginPlay Character HUD has no widgets defined"));
		}
		
		return;
	}
	CreateCharacterHUD(CharacterHUDWidgetClasses);
}

void ABattleRoyaleHUD::CreateCharacterHUD(TArray<TSubclassOf<UUserWidget>> widgetClasses)
{
	FActorSpawnParameters spawnInfo;
	spawnInfo.Owner = this;
	//spawnInfo.Instigator = this;
	spawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	mCharacterHUD = GetWorld()->SpawnActor<ACharacterHUD>(
		ACharacterHUD::StaticClass(),
		FVector::ZeroVector,
		FRotator::ZeroRotator,
		spawnInfo);
	mCharacterHUD->Initialize(0, GetOwningPlayerController(), widgetClasses);
}