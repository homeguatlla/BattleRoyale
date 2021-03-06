#pragma once
#include "BattleRoyaleStates.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRModeFSM
{
	class BattleRoyaleContext;

	class Synchronize : public core::utils::FSM::BaseState<BattleRoyaleState, BattleRoyaleContext>
	{
	public:
		
		Synchronize();
		virtual ~Synchronize() = default;

		BattleRoyaleState GetID() const override { return BattleRoyaleState::STATE_SYNCHRONIZE; }
		
		virtual void OnEnter(float deltaTime) override;
		virtual void OnExit(float deltaTime) override;
	};
};
