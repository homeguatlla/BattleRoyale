
#pragma once
#include <functional>

#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS


class ACharacterMock : public ACharacterBase
{
public:
	ACharacterMock() = default;

	void Initialize(float health)
	{
		SetCurrentHealth(health);
	}
};

class APlayerStateMock : public APlayerStateBase
{
public:
	APlayerStateMock() = default;

	void Initialize(int teamId)
	{
		SetTeamId(teamId);
	}
};

class GameStateMock : public ABattleRoyaleGameState
{
public:
	GameStateMock()
	{
	}

	void Initialize(int numPlayers, int numTeams)
	{
		int teamId = 0;

		for (auto player = 0; player < numPlayers; player++)
		{
			const auto playerState = NewObject<APlayerStateMock>();
			playerState->Initialize(teamId);

			const auto character = NewObject<ACharacterMock>();
			character->Initialize(100);

			character->SetPlayerState(playerState);
			
			PlayerArray.Add(playerState);
			teamId = (teamId + 1) % numTeams;
		}
	}

	IICharacter* GetCharacter(int index)
	{
		auto playerState = Cast<APlayerStateMock>(PlayerArray[index]);
		IICharacter* character = Cast<IICharacter>(playerState->GetPawn());

		return character;
	}
	
	virtual bool DidCountdownStart() const override { return false; }
	virtual bool DidCountdownFinish() const override { return false; }

	virtual void StartCountdownServer(int duration) override
	{
	}

	virtual void
	PerformActionForEachPlayerState(std::function<bool(const IIPlayerState* playerState)> action) const override
	{
		//TODO pensar esto. En teoría creo que lo ideal sería poder agregar player states con datos
		//pero eso es más complicado creo.
		//Aquí deberíamos definir un array de player states y ejecutar las acciones
		//si es posible rellenar le propio player state??
		ABattleRoyaleGameState::PerformActionForEachPlayerState(action);
	}
};

#endif
#endif
