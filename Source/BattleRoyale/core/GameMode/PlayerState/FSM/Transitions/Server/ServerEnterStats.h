#pragma once
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerEnterStats : public core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>
	{
	public:
		ServerEnterStats(StatePtr origin, StatePtr destination);
		virtual ~ServerEnterStats() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		IIGameState* mGameState;
	};
};
