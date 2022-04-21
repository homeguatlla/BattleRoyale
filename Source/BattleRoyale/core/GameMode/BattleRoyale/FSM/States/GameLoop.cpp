#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/GameLoop.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"


namespace BRModeFSM
{
	GameLoop::GameLoop()
	{
	}

	void GameLoop::OnInit()
	{
		mGameState = GetContext()->GetGameState();
		InitializeGameRules();
		
	}

	void GameLoop::OnEnter(float deltaTime)
	{
		
	}

	void GameLoop::OnUpdate(float deltaTime)
	{
		mGameRules.Execute();
	}

	void GameLoop::InitializeGameRules()
	{
		const auto checkThereIsOnlyOneTeamAliveRule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
		checkThereIsOnlyOneTeamAliveRule->Initialize(mGameState);
		
		mGameRules.AddRule(checkThereIsOnlyOneTeamAliveRule);
	}
};