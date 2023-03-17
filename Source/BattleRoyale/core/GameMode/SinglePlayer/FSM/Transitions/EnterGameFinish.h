#pragma once
#include "BattleRoyale/core/GameMode/SinglePlayer/FSM/States/SinglePlayerStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

class IIGameState;

namespace BRModeFSM
{
	class SinglePlayerContext;

	class EnterGameFinish : public core::utils::FSM::BaseTransition<SinglePlayerState, SinglePlayerContext>
	{
	public:
		EnterGameFinish(StatePtr origin, StatePtr destination);
		virtual ~EnterGameFinish() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;
		
	private:
		IIGameState* mGameState = nullptr;
	};
};
