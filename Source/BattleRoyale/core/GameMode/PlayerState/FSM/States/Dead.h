#pragma once
#include "PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class Dead : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		Dead() = default;
		virtual ~Dead() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_DEAD; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
	};
};
