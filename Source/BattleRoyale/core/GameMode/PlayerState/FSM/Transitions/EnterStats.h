#pragma once
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class EnterStats : public core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>
	{
	public:
		EnterStats(StatePtr origin, StatePtr destination);
		virtual ~EnterStats() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		IIGameState* mGameState;
	};
};
