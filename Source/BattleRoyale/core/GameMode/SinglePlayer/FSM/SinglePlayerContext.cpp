
#include "SinglePlayerContext.h"

namespace BRModeFSM
{
	SinglePlayerContext::SinglePlayerContext(UWorld* world, const IIGameMode* gameMode, IIGameState* gameState) :
	FSMContextBase(world),
	mGameMode{ gameMode },
	mGameState{ gameState }
	{
		/*auto gameMode = world->GetAuthGameMode<ABountyHunterGameMode>();
		if (gameMode->IsValidLowLevel())
		{
			mEventDispatcher = gameMode->GetEventDispatcher();
		}*/
	}
}
