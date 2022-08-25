// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStateBase.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "BattleRoyale/BattleRoyaleGameInstance.h"
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/GameModeCommon.h"
#include "BattleRoyale/core/GameMode/MultiplayerGameMode.h"
#include "BattleRoyale/core/Utils/FSM/StatesMachineFactory.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"


APlayerStateBase::APlayerStateBase() :
mTeamId(0),
mNumKills(0),
mDidWin(false),
mPlayerInteraction("")
{
	mAbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>(TEXT("AbilitySystemComponent"));
	mAbilitySystemComponent->SetIsReplicated(true);
	//mAbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(false);
}

void APlayerStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	mStatesMachineController.Update(DeltaSeconds);
}

void APlayerStateBase::Initialize(const UGameModeConfigurationInfo* gameModeConfiguration)
{
	mGameModeConfiguration = gameModeConfiguration;
}

bool APlayerStateBase::IsAlive() const
{
	if(const auto character = GetCharacter())
	{
		return character->IsAlive();
	}
	
	return false;
}

bool APlayerStateBase::IsPlayerReplicated() const
{
	if(const auto character = GetCharacter())
	{
		return character->IsCharacterValid();
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

void APlayerStateBase::PlayerInteraction(const FString& action)
{
	mPlayerInteraction = action;
}

FString APlayerStateBase::GetPlayerInteraction() const
{
	return mPlayerInteraction;
}

void APlayerStateBase::ResetPlayerInteraction()
{
	mPlayerInteraction.Empty();
}

void APlayerStateBase::NotifyRefreshHealth(float health) const
{
	ClientRefreshHealth(health);
}

void APlayerStateBase::NotifyAnnouncementOfNewDeathToAll(const FString& killerName, const FString& victimName) const
{
	MulticastAnnouncementOfNewDeath(killerName, victimName);
}

void APlayerStateBase::NotifyNumKillsToSelf()
{
	ClientRefreshNumKills(GetNumKills());
}

void APlayerStateBase::NotifyGameOverServer(bool hasMatchEnded, bool isWinner)
{
	if(isWinner)
	{
		SetAsWinner();
	}
	
	if(hasMatchEnded)
	{
		//TODO enter spectator mode	
	}
	ClientNotifyGameOver(isWinner);
}

void APlayerStateBase::NotifyNumTeamsAndPlayersAlive(uint8 numTeams, uint8 numPlayers)
{
	ClientNotifyTeamsAndPlayersAlive(numTeams, numPlayers);
}

void APlayerStateBase::ShowVictoryScreen() const
{
	if(GetPawn()->IsLocallyControlled())
	{
		GetEventDispatcher()->OnAnnouncePlayerWon.Broadcast();
	}
}

void APlayerStateBase::ShowDeathScreen() const
{
	if(GetPawn()->IsLocallyControlled())
	{
		GetEventDispatcher()->OnPlayerDead.Broadcast();
	}
}

void APlayerStateBase::OnGameStarted()
{
	CreateStatesMachine();
	SetActorTickEnabled(true);
}

void APlayerStateBase::ClientNotifyGameOver_Implementation(bool isWinner)
{
	if(isWinner)
	{
		SetAsWinner();
	}
	
	//Notify game over event
	GetEventDispatcher()->OnGameOver.Broadcast();
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
	
	GetEventDispatcher()->OnShowStatsScreen.Broadcast(data);
}

void APlayerStateBase::HideStatsScreen() const
{
	if(!GetPawn()->IsLocallyControlled())
	{
		return;
	}
	GetEventDispatcher()->OnHideStatsScreen.Broadcast();
}

void APlayerStateBase::Restart()
{
	//TODO esto no me gusta. Tendría que estar quizá en el gameinstance
	//además que la ruta del mapa está hardcodeada. No creo que se pueda obtener del gamemode.
	//Hay que poner esta información en un dataasset de configuración de gamemode??
	AGameModeCommon::DestroyGameSession();
	auto multiplayerConfiguration = mGameModeConfiguration->GetMultiplayerConfigurationInfo();
	auto levelMap = multiplayerConfiguration->GetMapsPath().ToString() + multiplayerConfiguration->GetMainMapName().ToString();
	UGameplayStatics::OpenLevel(this, FName(levelMap), true);
}

void APlayerStateBase::ForceFSMStateClient(BRPlayerStateFSM::PlayerStateState state)
{
	ClientForceFSMState(static_cast<int>(state));
}

void APlayerStateBase::BeginDestroy()
{
	if(!HasAuthority())
	{
		AGameModeCommon::DestroyGameSession();
	}
	Super::BeginDestroy();
}

void APlayerStateBase::ClientRefreshHealth_Implementation(float health) const
{
	GetEventDispatcher()->OnRefreshHealth.Broadcast(health);
}

void APlayerStateBase::ClientRefreshNumKills_Implementation(int numKills)
{
	mNumKills = numKills;
	GetEventDispatcher()->OnRefreshNumKills.Broadcast(numKills);
}

void APlayerStateBase::MulticastAnnouncementOfNewDeath_Implementation(const FString& killerName, const FString& victimName) const
{
	GetEventDispatcher()->OnAnnounceNewDeath.Broadcast(killerName, victimName);
}

void APlayerStateBase::ClientForceFSMState_Implementation(int state)
{
	mStatesMachineController.ForceState(0, static_cast<BRPlayerStateFSM::PlayerStateState>(state));
}

void APlayerStateBase::ClientNotifyTeamsAndPlayersAlive_Implementation(uint8 numTeams, uint8 numPlayers)
{
	GetEventDispatcher()->OnAnnounceTeamsAndPlayersAlive.Broadcast(numTeams, numPlayers);
}

void APlayerStateBase::CreateStatesMachine()
{
	mPlayerStateFSMContext = std::make_shared<BRPlayerStateFSM::PlayerStateContext>(
		GetWorld(),
		this,
		GetCharacter(),
		GetEventDispatcher());
	
	BattleRoyale::StatesMachineFactory factory;
	
	const auto fsmType = HasAuthority() ? FSMType::PLAYER_STATE_SERVER : FSMType::PLAYER_STATE_CLIENT;
	
	mStatesMachineController.AddMachine(
		std::move(factory.CreatePlayerStateFSM(fsmType, mPlayerStateFSMContext)));
}

APlayerController* APlayerStateBase::GetPlayerController() const
{
	if(GetPawn())
	{
		if(auto controller = GetPawn()->GetController())
		{
			return Cast<APlayerController>(controller);
		}
	}
	return nullptr;
}

UEventDispatcher* APlayerStateBase::GetEventDispatcher() const
{
	const auto gameInstance = Cast<UBattleRoyaleGameInstance>(GetGameInstance());
	return gameInstance->GetEventDispatcher();
}
