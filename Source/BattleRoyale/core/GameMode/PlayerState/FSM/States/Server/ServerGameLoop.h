#pragma once
#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/PlayerStateStates.h"
#include "BattleRoyale/core/Utils/FSM/fsm/BaseState.h"


namespace BRPlayerStateFSM
{
	class PlayerStateContext;

	class ServerGameLoop : public core::utils::FSM::BaseState<PlayerStateState, PlayerStateContext>
	{
	public:
		ServerGameLoop() = default;
		virtual ~ServerGameLoop() = default;

		PlayerStateState GetID() const override { return PlayerStateState::STATE_GAMELOOP_SERVER; }
		
		virtual void OnInit() override;
		virtual void OnEnter(float deltaTime) override;
		
	private:
		void OnTimeOver() const;

		IICharacter* mCharacter;
	};
};
