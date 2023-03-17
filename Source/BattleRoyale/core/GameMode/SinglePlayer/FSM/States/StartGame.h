#pragma once
#include "SinglePlayerStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class SinglePlayerContext;

	class StartGame : public core::utils::FSM::BaseState<SinglePlayerState, SinglePlayerContext>
	{
	public:
		StartGame();
		virtual ~StartGame() override = default;

		virtual SinglePlayerState GetID() const override { return SinglePlayerState::STATE_INIT; }
		
		virtual void OnInit() override;
	};
};
