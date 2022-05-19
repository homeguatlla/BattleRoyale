#pragma once
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ClientRestart : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		ClientRestart() = default;
		virtual ~ClientRestart() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_RESTART_CLIENT; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
	};
};
