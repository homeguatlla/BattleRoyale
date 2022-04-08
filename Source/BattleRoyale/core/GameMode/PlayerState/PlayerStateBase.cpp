// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Character/ICharacter.h"

APlayerStateBase::APlayerStateBase() : mTeamId(0), mNumKills(0)
{
	mAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	mAbilitySystemComponent->SetIsReplicated(true);
}

bool APlayerStateBase::IsAlive() const
{
	if(GetPawn() && GetPawn()->Implements<UICharacter>())
	{
		//TODO quizá podríamos pillar la vida del GAS cuando sea un atributo
		const auto character = Cast<IICharacter>(GetPawn());
		return character->IsAlive();
	}
	
	return false;
}

void APlayerStateBase::NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const
{
	MulticastAnnouncementOfNewDeath(killerName, victimName);
}

void APlayerStateBase::NotifyNumKillsToSelf() const
{
	ClientRefreshNumKills(GetNumKills());
}

void APlayerStateBase::NotifyAnnouncementOfWinner() const
{
	ClientNotifyWinner();
}

void APlayerStateBase::ClientNotifyWinner_Implementation() const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnAnnouncePlayerWon.Broadcast();
}

void APlayerStateBase::ClientRefreshNumKills_Implementation(int numKills) const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnRefreshNumKills.Broadcast(numKills);
}

void APlayerStateBase::MulticastAnnouncementOfNewDeath_Implementation(const FString& killerName, const FString& victimName) const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnAnnounceNewDeath.Broadcast(killerName, victimName);
}
