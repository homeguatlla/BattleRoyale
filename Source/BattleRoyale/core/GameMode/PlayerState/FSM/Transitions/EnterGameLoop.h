#pragma once
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class EnterGameLoop : public core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>
	{
	public:
		EnterGameLoop(StatePtr origin, StatePtr destination);
		virtual ~EnterGameLoop() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		IIPlayerState* mPlayerState;
	};
};
