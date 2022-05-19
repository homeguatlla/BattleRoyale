#pragma once
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"

class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerGameOver : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		
		ServerGameOver() = default;
		virtual ~ServerGameOver() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_GAME_OVER_SERVER; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		
	private:
		IIPlayerState* mPlayerState;
	};
};
