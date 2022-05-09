
#include "ClientVictory.h"

#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	void ClientVictory::OnInit()
	{
		mEventDispatcher = GetContext()->GetEventDispatcher();
	}
	
	void ClientVictory::OnEnter(float deltaTime)
	{
		GetContext()->GetPlayerState()->ShowVictoryScreen();
	}
};