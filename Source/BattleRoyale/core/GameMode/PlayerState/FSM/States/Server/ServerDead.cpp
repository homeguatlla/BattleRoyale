
#include "ServerDead.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	void ServerDead::OnInit()
	{
		mCharacter = GetContext()->GetCharacter();
		mPlayerState = GetContext()->GetPlayerState();
	}
	
	void ServerDead::OnEnter(float deltaTime)
	{
		mCharacter->DieServer();
		mPlayerState->ShowDeathScreen();
		
		GetContext()->StartTimer(mPlayerState->GetTimeBetweenEndOfGameAndStatsScreen());
	}
};