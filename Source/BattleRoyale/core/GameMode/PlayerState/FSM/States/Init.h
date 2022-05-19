#pragma once
#include "PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class Init : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		Init() = default;
		virtual ~Init() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_INIT; }
		
		virtual void OnInit() override;
	};
};
