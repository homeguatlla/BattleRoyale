#pragma once

#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerVictory : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		ServerVictory() = default;
		virtual ~ServerVictory() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_VICTORY_SERVER; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
	};
};
