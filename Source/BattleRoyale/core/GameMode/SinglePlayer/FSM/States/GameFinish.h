#pragma once
#include "SinglePlayerStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class SinglePlayerContext;

	class GameFinish : public core::utils::FSM::BaseState<SinglePlayerState, SinglePlayerContext>
	{
	public:
		
		GameFinish();
		virtual ~GameFinish() override = default;

		virtual SinglePlayerState GetID() const override { return SinglePlayerState::STATE_MATCHEND; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
	};
};
