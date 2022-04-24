// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGameMode.h"
#include "MultiplayerGameMode.h"
#include "GameModeCommon.generated.h"

class IIPlayerController;
class APlayerStateBase;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API AGameModeCommon : public AMultiplayerGameMode, public IIGameMode
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;	
	
	//Inherit from IIGameMode
	virtual IIGameState* GetGameState() const override;
	virtual void OnNewKill(const APlayerController* killerController, const APlayerController* victimController) override;
	virtual bool CanPlayerCauseDamageTo(const APlayerController* killerController, const APlayerController* victimController) override;
	
private:
	virtual void GenericPlayerInitialization(AController* controller) override;
	virtual bool ReadyToStartMatch_Implementation() override;
	void DisableControllerInput(AController* controller) const;
	IIPlayerController* GetPlayerController(AController* controller) const;
	
	void NotifyNewKillToAll(const APlayerController* victimController, APlayerStateBase* playerStateKiller) const;
};
