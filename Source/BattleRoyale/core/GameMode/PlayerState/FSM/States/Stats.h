#pragma once
#include "PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"

class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class Stats : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		Stats() = default;
		virtual ~Stats() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_STATS; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;

	private:
		IIPlayerState* mPlayerState; 	
	};
};
