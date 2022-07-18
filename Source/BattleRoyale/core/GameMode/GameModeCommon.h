// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "IGameMode.h"
#include "MultiplayerGameMode.h"
#include "Configuration/GameModeConfigurationInfo.h"
#include "GameModeCommon.generated.h"

class IIPlayerState;
class IIPlayerController;
class APlayerStateBase;
class UMultiplayerConfigurationInfo;

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

	//Configuration
	virtual float GetGameRulesUpdateIntervalTime() const override { return ConfigurationInfo->GetGameRulesUpdateIntervalTime(); }
	
	//Inherit from IIGameMode
	virtual IIGameState* GetGameState() const override;
	virtual void OnNewKill(IIPlayerState* killerPlayerState, IIPlayerState* victimPlayerState) override;
	virtual bool CanPlayerCauseDamageTo(const IIPlayerState* causerPlayerState, const IIPlayerState* victimPlayerState) override;
	virtual bool HasCurrentGameSession() const override;
	virtual const UGameModeConfigurationInfo* GetGameModeConfiguration() const override { return ConfigurationInfo;}
	
	static void DestroyGameSession();
protected:
	UPROPERTY(EditAnywhere)
	UGameModeConfigurationInfo* ConfigurationInfo;
	
private:
	virtual void GenericPlayerInitialization(AController* controller) override;
	virtual bool ReadyToStartMatch_Implementation() override;
	void DisableControllerInput(AController* controller) const;
	IIPlayerController* GetPlayerController(AController* controller) const;
	
	void NotifyNewKillToAll(const IIPlayerState* victimPlayerState, const IIPlayerState* playerStateKiller) const;
};
