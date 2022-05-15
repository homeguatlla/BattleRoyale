#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/BattleRoyaleContext.h>

namespace BRModeFSM
{
	BattleRoyaleContext::BattleRoyaleContext(UWorld* world, const IIGameMode* gameMode, IIGameState* gameState, float countdownTimeToStartGame) :
	FSMContextBase(world),
	mGameMode{ gameMode },
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