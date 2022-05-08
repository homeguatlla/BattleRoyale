// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"


APlayerStateBase::APlayerStateBase() : mTeamId(0), mNumKills(0), mDidWin(false)
{
	mAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	mAbilitySystemComponent->SetIsReplicated(true);
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void APlayerStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if(HasAuthority())
	{
		mStatesMachineController.Update(DeltaSeconds);
	}
}

bool APlayerStateBase::IsAlive() const
{
	if(const auto character = GetCharacter())
	{
		//TODO quizá podríamos pillar la vida del GAS cuando sea un atributo
		return character->IsAlive();
	}
	
	return false;
}

IICharacter* APlayerStateBase::GetCharacter() const
{
	if(GetPawn() && GetPawn()->Implements<UICharacter>())
	{
		return Cast<IICharacter>(GetPawn());
	}
	
	return nullptr;
}

void APlayerStateBase::SetAsWinner()
{
	mDidWin = true;
}

bool APlayerStateBase::DidPlayerWin() const
{
	return mDidWin;
}

void APlayerStateBase::NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const
{
	MulticastAnnouncementOfNewDeath(killerName, victimName);
}

void APlayerStateBase::NotifyNumKillsToSelf()
{
	ClientRefreshNumKills(GetNumKills());
}

void APlayerStateBase::NotifyAnnouncementOfWinner() const
{
	ClientNotifyWinner();
}

void APlayerStateBase::NotifyGameOver() const
{
	ClientNotifyGameOver();
}

void APlayerStateBase::OnGameStarted()
{
	//We are sure all pawns are created
	//if(HasAuthority())
	{
		CreateStatesMachine();
		SetActorTickEnabled(true);
	}
}

void APlayerStateBase::ClientNotifyGameOver_Implementation() const
{
	//Notify game over event
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnGameOver.Broadcast();
}

void APlayerStateBase::ShowStatsScreen() const
{
	if(!GetPawn()->IsLocallyControlled())
	{
		return;
	}
	//TODO no sé si sería mejor poner las stats dentro del gameover y listos
	//Porque este evento de stats screen es un poco raro, no es genérico de la IPlayerState
	//Notify game stats event
	FPlayerStatsData data;
	data.mNumKills = mNumKills;
	data.mTeamId = mTeamId;
	data.mPlayerName = GetPlayerNickName();
	
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnShowStatsScreen.Broadcast(data);
}

void APlayerStateBase::ForceFSMStateClient(BRPlayerStateFSM::PlayerStateState state)
{
	ClientForceFSMState(static_cast<int>(state));
}

void APlayerStateBase::ClientNotifyWinner_Implementation() const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnAnnouncePlayerWon.Broadcast();
}

void APlayerStateBase::ClientRefreshNumKills_Implementation(int numKills)
{
	mNumKills = numKills;
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnRefreshNumKills.Broadcast(numKills);
}

void APlayerStateBase::MulticastAnnouncementOfNewDeath_Implementation(const FString& killerName, const FString& victimName) const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	gameInstance->GetEventDispatcher()->OnAnnounceNewDeath.Broadcast(killerName, victimName);
}

void APlayerStateBase::ClientForceFSMState_Implementation(int state)
{
	mStatesMachineController.ForceState(0, static_cast<BRPlayerStateFSM::PlayerStateState>(state));
}

void APlayerStateBase::CreateStatesMachine()
{
	mPlayerStateFSMContext = std::make_shared<BRPlayerStateFSM::PlayerStateContext>(GetWorld(), this, GetCharacter());
	
	BattleRoyale::StatesMachineFactory factory;
	
	const auto fsmType = HasAuthority() ? FSMType::PLAYER_STATE_SERVER : FSMType::PLAYER_STATE_CLIENT;
		
	mStatesMachineController.AddMachine(
		std::move(factory.CreatePlayerStateFSM(fsmType, mPlayerStateFSMContext)));
}
