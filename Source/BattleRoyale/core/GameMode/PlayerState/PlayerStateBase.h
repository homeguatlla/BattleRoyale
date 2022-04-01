// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameplayAbilitySystem/AbilitySystemComponentBase.h"
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
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return mAbilitySystemComponent; }
	virtual IIAbilitySystemInterfaceBase* GetAbilitySystemComponentInterface() const override { return Cast<IIAbilitySystemInterfaceBase>(mAbilitySystemComponent); }
	virtual bool IsAlive() const override;
	virtual void SetTeamId(int teamId) override { mTeamId = teamId; }
	virtual int GetTeamId() const override { return mTeamId; }
	virtual FString GetPlayerNickName() const override { return GetPlayerName(); }
	virtual void NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const override;
	
private:
	UFUNCTION(NetMulticast, Unreliable)
	void MulticastAnnouncementOfNewDeath(const FString& killerName, const FString& victimName) const;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = AbilitySystemComponent, meta = (AllowPrivateAccess = "true"))
	UAbilitySystemComponentBase* mAbilitySystemComponent;

	int mTeamId;
};
