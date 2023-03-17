

#include "GameFinish.h"

#include "BattleRoyale/core/GameMode/SinglePlayer/SinglePlayerGameMode.h"
#include "BattleRoyale/core/GameMode/SinglePlayer/FSM/SinglePlayerContext.h"

namespace BRModeFSM
{
	GameFinish::GameFinish()
	{
	}

	void GameFinish::OnInit()
	{
	}

	void GameFinish::OnEnter(float deltaTime)
	{
		/*const auto singlePlayerGameMode = Cast<ASinglePlayerGameMode>(GetContext()->GetGameMode());		
		GetContext()->StartTimer(singlePlayerGameMode->GetOnceMatchEndedTimeToResetSessionTime());*/
	}
};