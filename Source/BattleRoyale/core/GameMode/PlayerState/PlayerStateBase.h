// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/Configuration/GameModeConfigurationInfo.h"
#include "BattleRoyale/core/GameplayAbilitySystem/AbilitySystemComponentBase.h"
#include "BattleRoyale/core/Utils/EventDispatcher.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "FSM/PlayerStateContext.h"
#include "FSM/States/PlayerStateStates.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"


class UAttributeSetHealth;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API APlayerStateBase : public APlayerState, public IIPlayerState
{
	GENERATED_BODY()

public:
	
	APlayerStateBase();
	virtual void Tick(float DeltaSeconds) override;

	virtual void Initialize(const UGameModeConfigurationInfo* gameModeConfiguration) override;
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return mAbilitySystemComponent; }
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentInterface() const override { return Cast<IIAbilitySystemInterfaceBase>(mAbilitySystemComponent); }
	virtual bool IsAlive() const override;
	
	virtual bool IsPawnReplicated() const override { return GetPawn() != nullptr; }
	virtual void SetTeamId(int teamId) override { mTeamId = teamId; }
	virtual int GetTeamId() const override { return mTeamId; }
	virtual FString GetPlayerNickName() const override { return GetPlayerName(); }
	virtual void AddKill() override { mNumKills++; }
	virtual int GetNumKills() const override { return mNumKills; }

	virtual float GetCurrentHealth() override { return 100.0f; } //return mGameplayAbilityAttributes->GetHealth(); }
	
	virtual void SetAsWinner() override;
	virtual bool DidPlayerWin() const override;

	virtual void PlayerInteraction(const FString& action) override;
	virtual FString GetPlayerInteraction() const override;
	virtual void ResetPlayerInteraction() override;
	
	virtual IICharacter* GetCharacter() const override;
	
	virtual void NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const override;
	virtual void NotifyNumKillsToSelf() override;
	virtual void NotifyGameOverServer(bool hasMatchEnded, bool isWinner) override;
	virtual void NotifyNumTeamsAndPlayersAlive(uint8 numTeams, uint8 numPlayers) override;
	
	virtual void OnGameStarted() override;
	virtual void ShowStatsScreen() const override;
	virtual void HideStatsScreen() const override;

	virtual float GetTimeBetweenEndOfGameAndStatsScreen() const override { return mGameModeConfiguration->GetTimeBetweenEndOfGameAndStatsScreen(); }
	
	virtual void ShowVictoryScreen() const override;
	virtual void ShowDeathScreen() const override;

	virtual void Restart() override;
	virtual void ForceFSMStateClient(BRPlayerStateFSM::PlayerStateState state) override;

	virtual void BeginDestroy() override;
private:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAnnouncementOfNewDeath(const FString& killerName, const FString& victimName) const;

	UFUNCTION(Client, Unreliable)
	void ClientRefreshNumKills(int numKills);
	
	UFUNCTION(Client, Unreliable)
	void ClientNotifyGameOver(bool isWinner);

	UFUNCTION(Client, Reliable)
	void ClientForceFSMState(int state);

	UFUNCTION(Client, Unreliable)
	void ClientNotifyTeamsAndPlayersAlive(uint8 numTeams, uint8 numPlayers);
	
	void CreateStatesMachine();
	
	APlayerController* GetPlayerController() const;
	UEventDispatcher* GetEventDispatcher() const;
	
	UPROPERTY()
	const UGameModeConfigurationInfo* mGameModeConfiguration;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AbilitySystemComponent, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponentBase* mAbilitySystemComponent;
	
	int mTeamId;
	int mNumKills;
	bool mDidWin;

	//States machine to control the player state
	StatesMachineController<BRPlayerStateFSM::PlayerStateState, BRPlayerStateFSM::PlayerStateContext> mStatesMachineController;
	std::shared_ptr<BRPlayerStateFSM::PlayerStateContext> mPlayerStateFSMContext;
	
	FString mPlayerInteraction;
};
