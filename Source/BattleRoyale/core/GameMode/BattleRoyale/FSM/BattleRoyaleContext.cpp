#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h>

namespace BRModeFSM
{
	BattleRoyaleContext::BattleRoyaleContext(UWorld* world, IIGameState* gameState, float countdownTimeToStartGame) :
	mWorld{ world },
	mGameState{ gameState },
	mCountdownTimeToStartGame{ countdownTimeToStartGame}
	{
		/*auto gameMode = world->GetAuthGameMode<ABountyHunterGameMode>();
		if (gameMode->IsValidLowLevel())
		{
			mEventDispatcher = gameMode->GetEventDispatcher();
		}*/
	}
}