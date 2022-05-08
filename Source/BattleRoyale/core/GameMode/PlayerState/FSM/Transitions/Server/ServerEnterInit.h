#pragma once
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseTransition.h"

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerEnterInit : public core::utils::FSM::BaseTransition<PlayerStateState, PlayerStateContext>
	{
	public:
		ServerEnterInit(StatePtr origin, StatePtr destination);
		virtual ~ServerEnterInit() = default;

		void OnInit() override;
		bool CanPerformTransition() const override;

	private:
		IICharacter* mCharacter;
	};
};
