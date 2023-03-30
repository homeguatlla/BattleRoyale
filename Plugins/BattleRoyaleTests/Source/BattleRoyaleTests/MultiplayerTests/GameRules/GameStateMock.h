
#pragma once
#include <functional>

#include "CoreMinimal.h"
#include "CharacterMock.h"
#include "MockClasses.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameState.h"
#include "GameStateMock.generated.h"

UCLASS()
class AGameStateMock : public ABattleRoyaleGameState
{
	GENERATED_BODY()
public:
	AGameStateMock()
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
			
			character->SetCurrentHealthTest(100);

			character->SetPlayerState(playerState);
			
			PlayerArray.Add(playerState);
			teamId = (teamId + 1) % numTeams;
		}
	}
	virtual bool AreAllPlayersReplicated() const override
	{
		return true;
	}

	IICharacter* GetCharacter(int index)
	{
		const auto playerState = Cast<APlayerStateMock>(PlayerArray[index]);
		const auto character = Cast<IICharacter>(playerState->GetPawn());

		return character;
		return nullptr;
	}
/*
	virtual void
	PerformActionForEachPlayerState(std::function<bool(IIPlayerState* playerState)> action) const override
	{
		//TODO pensar esto. En teoría creo que lo ideal sería poder agregar player states con datos
		//pero eso es más complicado creo.
		//Aquí deberíamos definir un array de player states y ejecutar las acciones
		//si es posible rellenar le propio player state??
		ABattleRoyaleGameState::PerformActionForEachPlayerState(action);
	}*/
};