
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Reset.h"

#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/IMultiplayerGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	Reset::Reset()
	{
	}

	void Reset::OnInit()
	{
		mGameState = Cast<IIMultiplayerGameState>(GetContext()->GetGameState());
	}

	void Reset::OnEnter(float deltaTime)
	{
		mGameState->CloseAllPlayersGameSessionServer();

		//Quizá sería mejor hacer un RestartGame
		//GameMode restartGame.
	}
};