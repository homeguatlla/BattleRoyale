
#include "ClientStats.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"

namespace BRPlayerStateFSM
{
	void ClientStats::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	void ClientStats::OnEnter(float deltaTime)
	{
		mPlayerState->ShowStatsScreen();
	}
};