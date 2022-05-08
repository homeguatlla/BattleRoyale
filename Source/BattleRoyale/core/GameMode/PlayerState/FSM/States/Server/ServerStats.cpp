
#include "ServerStats.h"

#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"


namespace BRPlayerStateFSM
{
	void ServerStats::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	void ServerStats::OnEnter(float deltaTime)
	{
		mPlayerState->ForceFSMStateClient(BRPlayerStateFSM::PlayerStateState::STATE_STATS_CLIENT);
		mPlayerState->NotifyGameOver();
		mPlayerState->ShowStatsScreen();
	}
};