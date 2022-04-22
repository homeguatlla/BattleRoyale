#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


class IIGameState;

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class Countdown : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		
		Countdown();
		virtual ~Countdown() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_COUNTDOWN; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		virtual void OnExit(float deltaTime) override;
		
	private:
		IIGameState* mGameState;
	};
};
