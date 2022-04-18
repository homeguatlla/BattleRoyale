#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h>

namespace BRModeFSM
{
	BattleRoyaleContext::BattleRoyaleContext(UWorld* world) :
	mWorld{ world }
	{
		/*auto gameMode = world->GetAuthGameMode<ABountyHunterGameMode>();
		if (gameMode->IsValidLowLevel())
		{
			mEventDispatcher = gameMode->GetEventDispatcher();
		}*/
	}
}