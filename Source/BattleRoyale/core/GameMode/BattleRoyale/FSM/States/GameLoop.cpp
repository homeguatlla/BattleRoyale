#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/GameLoop.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"


namespace BRModeFSM
{
	GameLoop::GameLoop()
	{
	}

	void GameLoop::OnInit()
	{
		mGameState = GetContext()->GetGameState();
	}

	void GameLoop::OnEnter(float deltaTime)
	{
		
	}

	void GameLoop::OnUpdate(float deltaTime)
	{
		mGameRules.Execute();
	}
};