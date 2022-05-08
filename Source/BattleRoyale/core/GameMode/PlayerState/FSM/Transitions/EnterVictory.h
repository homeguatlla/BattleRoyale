#pragma once
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class EnterVictory : public core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>
	{
	public:
		EnterVictory(StatePtr origin, StatePtr destination);
		virtual ~EnterVictory() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		IIPlayerState* mPlayerState;
	};
};
