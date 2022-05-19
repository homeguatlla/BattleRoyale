
#include "ClientGameOver.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"

namespace BRPlayerStateFSM
{
	void ClientGameOver::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	void ClientGameOver::OnEnter(float deltaTime)
	{
		mPlayerState->ShowStatsScreen();
	}
};