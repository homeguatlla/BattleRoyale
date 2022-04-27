// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameplayAbilitySystem/AbilitySystemComponentBase.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineController.h"
#include "FSM/PlayerStateContext.h"
#include "FSM/States/PlayerStateStates.h"
#include "GameFramework/PlayerState.h"
#include "PlayerStateBase.generated.h"


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
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return mAbilitySystemComponent; }
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentInterface() const override { return Cast<IIAbilitySystemInterfaceBase>(mAbilitySystemComponent); }
	virtual bool IsAlive() const override;
	
	virtual bool IsPawnReplicated() const override { return GetPawn() != nullptr; }
	virtual void SetTeamId(int teamId) override { mTeamId = teamId; }
	virtual int GetTeamId() const override { return mTeamId; }
	virtual FString GetPlayerNickName() const override { return GetPlayerName(); }
	virtual void AddKill() override { mNumKills++; }
	virtual int GetNumKills() const override { return mNumKills; }

	virtual void NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const override;
	virtual void NotifyNumKillsToSelf() const override;
	virtual void NotifyAnnouncementOfWinner() const override;
	virtual void NotifyGameOver() const override;

	virtual void OnGameStarted() override;
	
private:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAnnouncementOfNewDeath(const FString& killerName, const FString& victimName) const;

	UFUNCTION(Client, Unreliable)
	void ClientRefreshNumKills(int numKills) const;

	UFUNCTION(Client, Unreliable)
	void ClientNotifyWinner() const;

	UFUNCTION(Client, Unreliable)
	void ClientNotifyGameOver() const;

	void CreateStatesMachineServer();
	IICharacter* GetCharacter() const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AbilitySystemComponent, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponentBase* mAbilitySystemComponent;

	int mTeamId;
	int mNumKills;

	//States machine to control the player state
	StatesMachineController<BRPlayerStateFSM::PlayerStateState, BRPlayerStateFSM::PlayerStateContext> mStatesMachineController;
	std::shared_ptr<BRPlayerStateFSM::PlayerStateContext> mGameStateFSMContext;
};
