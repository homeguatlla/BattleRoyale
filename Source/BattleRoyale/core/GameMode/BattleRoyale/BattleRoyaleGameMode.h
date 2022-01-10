// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameMode.h"
#include "BattleRoyaleGameMode.generated.h"

UCLASS(minimalapi)
class ABattleRoyaleGameMode : public AMultiplayerGameMode
{
	GENERATED_BODY()

public:
	ABattleRoyaleGameMode();
	void OnMatchStateChanged(FName matchState);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	TSubclassOf<APawn> mCharacter;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	uint8 mNumPlayersPerTeam = 1;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=BattleRoyale)
	float mCountdownTimeToStartGame = 15.f;

	virtual bool ReadyToStartMatch_Implementation() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	

protected:
	virtual void GenericPlayerInitialization(AController* controller) override;
	
private:
	//TODO esto seguramente podría estar definido en una interface
	bool HasGameStarted() const { return mHasGameStarted; }
	void TryToStartCountdown() const;
	IIGameState* GetGameState() const;
	
	TArray<AController*> mPlayerControllers;

	bool mHasGameStarted = false;
};



