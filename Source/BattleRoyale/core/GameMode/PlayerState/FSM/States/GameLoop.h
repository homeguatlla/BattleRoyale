#pragma once
#include "PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class GameLoop : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		GameLoop() = default;
		virtual ~GameLoop() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_GAMELOOP; }
		
		virtual void OnInit() override;
	};
};
