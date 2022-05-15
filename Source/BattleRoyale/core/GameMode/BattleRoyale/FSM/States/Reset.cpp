#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Reset.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h"

namespace BRModeFSM
{
	Reset::Reset()
	{
	}

	void Reset::OnInit()
	{
		mGameMode = GetContext()->GetGameMode();
	}

	void Reset::OnEnter(float deltaTime)
	{
		if(mGameMode->HasCurrentGameSession())
		{
			mGameMode->DestroyGameSession();
		}
	}
};