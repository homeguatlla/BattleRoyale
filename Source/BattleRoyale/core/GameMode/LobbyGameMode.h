// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGameState.h"
#include "GameFramework/GameMode.h"
#include "LobbyGameMode.generated.h"


UCLASS()
class BATTLEROYALE_API ALobbyGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="ConfigurationInfo")
	class UMultiplayerConfigurationInfo* mConfigurationInfo;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, DisplayName="NumPlayers")
	int mNumPlayers;
	
private:
	IIGameState* GetGameState() const;
};
