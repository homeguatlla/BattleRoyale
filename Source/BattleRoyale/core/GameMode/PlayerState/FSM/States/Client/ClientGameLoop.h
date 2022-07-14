#pragma once
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ClientGameLoop : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		ClientGameLoop() = default;
		virtual ~ClientGameLoop() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_GAMELOOP_CLIENT; }
		
		virtual void OnInit() override;
	};
};
