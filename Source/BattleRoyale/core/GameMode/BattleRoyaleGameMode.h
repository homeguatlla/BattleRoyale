// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "MultiplayerGameMode.h"
#include "BattleRoyaleGameMode.generated.h"

UCLASS(minimalapi)
class ABattleRoyaleGameMode : public AMultiplayerGameMode
{
	GENERATED_BODY()

public:
	ABattleRoyaleGameMode();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Classes)
	TSubclassOf<APawn> mCharacter;

protected:
	virtual void GenericPlayerInitialization(AController* controller) override;
};



