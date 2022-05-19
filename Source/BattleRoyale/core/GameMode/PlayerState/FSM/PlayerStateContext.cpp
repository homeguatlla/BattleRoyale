
#include "PlayerStateContext.h"

#include "BattleRoyale/core/GameMode/IPlayerState.h"

namespace BRPlayerStateFSM
{
	PlayerStateContext::PlayerStateContext(UWorld* world, IIPlayerState* playerState, IICharacter* character, UEventDispatcher* eventDispatcher) :
	FSMContextBase(world),
	mPlayerState { playerState },
	mCharacter { character },
	mEventDispatcher { eventDispatcher }
	{
	}
}
