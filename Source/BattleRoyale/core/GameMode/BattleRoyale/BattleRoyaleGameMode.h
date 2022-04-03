// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IGameMode.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameMode.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/PlayerController/IPlayerController.h"
#include "BattleRoyaleGameMode.generated.h"

UCLASS(minimalapi)
class ABattleRoyaleGameMode : public AMultiplayerGameMode, public IIGameMode
{
	GENERATED_BODY()

public:
	ABattleRoyaleGameMode();

	virtual void BeginPlay() override;
	void OnMatchStateChanged(FName matchState);
	virtual bool ReadyToStartMatch_Implementation() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	//Inherit from IIGameMode
	virtual void OnNewKill(const APlayerController* killerController, const APlayerController* victimController) override;
	virtual bool CanPlayerCauseDamageTo(const APlayerController* killerController, const APlayerController* victimController) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	TSubclassOf<APawn> mCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	uint8 mNumPlayersPerTeam = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	float mCountdownTimeToStartGame = 15.f;

protected:
	virtual void GenericPlayerInitialization(AController* controller) override;
	
private:
	//TODO esto seguramente podría estar definido en una interface
	bool HasGameStarted() const { return mHasGameStarted; }
	void TryToStartCountdown() const;
	void DisableControllerInput(AController* controller) const;
	IIPlayerController* GetPlayerController(AController* controller) const;
	IIGameState* GetGameState() const;

	void InitializeGameRules();
	void NotifyNewKillToAll(const APlayerController* victimController, APlayerStateBase* playerStateKiller) const;

	TArray<AController*> mPlayerControllers;

	bool mHasGameStarted = false;

	UPROPERTY()
	UGameRules* mGameRules;
};



