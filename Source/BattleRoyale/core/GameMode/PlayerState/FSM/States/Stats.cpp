
#include "Stats.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"

namespace BRPlayerStateFSM
{
	void Stats::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	void Stats::OnEnter(float deltaTime)
	{
		mPlayerState->NotifyGameOver();
	}
};