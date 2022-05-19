


#include "ClientRestart.h"

#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	void ClientRestart::OnInit()
	{
		
	}

	void ClientRestart::OnEnter(float deltaTime)
	{
		//GetContext()->GetPlayerState()->HideStatsScreen();
		GetContext()->GetPlayerState()->Restart();
	}
};