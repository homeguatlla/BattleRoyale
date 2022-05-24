#pragma once

#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


class IICharacter;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerDead : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		ServerDead() = default;
		virtual ~ServerDead() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_DEAD_SERVER; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;

	private:
		IICharacter* mCharacter;
		IIPlayerState* mPlayerState;
	};
};
