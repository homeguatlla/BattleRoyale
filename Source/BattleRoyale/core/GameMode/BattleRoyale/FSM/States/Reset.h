#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


class IIGameState;

namespace BRModeFSM
{
	class BattleRoyaleContext;

	class Reset : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		
		Reset();
		virtual ~Reset() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_RESTART; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;

	private:
		const IIGameState* mGameState;
	};
};
