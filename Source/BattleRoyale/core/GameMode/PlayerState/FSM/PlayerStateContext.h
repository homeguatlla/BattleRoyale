#pragma once
#include <functional>

#include "BattleRoyale/core/Utils/EventDispatcher.h"
#include "BattleRoyale/core/Utils/FSM/FSMContextBase.h"

class IICharacter;
class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext : public FSMContextBase
	{
	public:
		PlayerStateContext(UWorld* world, IIPlayerState* playerState, IICharacter* character, UEventDispatcher* eventDispatcher);

		~PlayerStateContext() = default;
		
		IIPlayerState* GetPlayerState() const { return mPlayerState; }
		IICharacter* GetCharacter() const { return mCharacter; }
		UEventDispatcher* GetEventDispatcher() const { return mEventDispatcher; }
	
	private:
		
		IIPlayerState* mPlayerState;
		IICharacter* mCharacter;
		UEventDispatcher* mEventDispatcher;
	};
};
