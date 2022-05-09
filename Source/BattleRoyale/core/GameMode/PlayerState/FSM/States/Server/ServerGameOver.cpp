
#include "ServerGameOver.h"

#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"


namespace BRPlayerStateFSM
{
	void ServerGameOver::OnInit()
	{
		mPlayerState = GetContext()->GetPlayerState();
	}

	void ServerGameOver::OnEnter(float deltaTime)
	{
		mPlayerState->ForceFSMStateClient(BRPlayerStateFSM::PlayerStateState::STATE_GAME_OVER_CLIENT);
		mPlayerState->ShowStatsScreen();
	}
};