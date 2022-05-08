
#include "ClientEnterInit.h"

#include "BattleRoyale/core/GameMode/PlayerState/FSM/PlayerStateContext.h"

namespace BRPlayerStateFSM
{
	ClientEnterInit::ClientEnterInit(StatePtr origin, StatePtr destination) :
		core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>(origin, destination)
	{
	}

	void ClientEnterInit::OnInit()
	{
		mCharacter = GetContext()->GetCharacter();
	}

	bool ClientEnterInit::CanPerformTransition() const
	{
		const auto result = mCharacter->AnyKeyPressed();
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Green, FString::Printf(TEXT("EnterInit anykeypressed?: %d"), result));
		return mCharacter->AnyKeyPressed();
	}	
};