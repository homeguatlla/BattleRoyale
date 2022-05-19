
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Reset.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	Reset::Reset()
	{
	}

	void Reset::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	void Reset::OnEnter(float deltaTime)
	{
		mGameState->CloseAllPlayersGameSessionServer();
	}
};