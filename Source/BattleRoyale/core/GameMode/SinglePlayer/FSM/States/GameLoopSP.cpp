

#include "GameLoopSP.h"
#include "BattleRoyale/core/GameMode/SinglePlayer/FSM/SinglePlayerContext.h"

namespace BRModeFSM
{
	GameLoopSP::GameLoopSP()
	{
	}

	void GameLoopSP::OnInit()
	{
		mGameState = GetContext()->GetGameState();				
	}

	void GameLoopSP::OnEnter(float deltaTime)
	{
		InitializeGameRules();
		mGameState->StartGameServer();
	}

	void GameLoopSP::OnUpdate(float deltaTime)
	{
		mGameRules.Execute();
	}

	void GameLoopSP::InitializeGameRules()
	{
		mGameRules.Reset();
	}
};