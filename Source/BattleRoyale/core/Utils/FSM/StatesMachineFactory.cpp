#include "StatesMachineFactory.h"

//FSM BattleRoyale States
#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Init.h>

//FSM BattleRoyale Transitions
#include <BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterCountdown.h>

#include "StatesMachineBuilder.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Countdown.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/GameLoop.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/MatchEnd.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Reset.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/States/Synchronize.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterGameLoop.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterMatchEnd.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterReset.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/FSM/Transitions/EnterSynchronize.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/GameLoop.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Init.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Client/ClientDead.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Client/ClientGameOver.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Client/ClientVictory.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Server/ServerDead.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Server/ServerGameOver.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/States/Server/ServerVictory.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/EnterDead.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/EnterGameLoop.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/EnterVictory.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/Client/ClientEnterInit.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/Server/ServerEnterInit.h"
#include "BattleRoyale/core/GameMode/PlayerState/FSM/Transitions/Server/ServerEnterGameOver.h"

namespace BattleRoyale
{
	std::unique_ptr<StatesMachineFactory::GameModeFSM> StatesMachineFactory::CreateModeFSM(
		FSMType type, 
		std::shared_ptr<BRModeFSM::BattleRoyaleContext> context)
	{
		StatesMachineBuilder<BRModeFSM::BattleRoyaleState, BRModeFSM::BattleRoyaleContext> builder;

		switch(type)
		{
			case FSMType::BATTLEROYALE_GAMEMODE:
				{
					const auto init = std::make_shared<BRModeFSM::Init>();
					const auto countdown = std::make_shared<BRModeFSM::Countdown>();
					const auto synchronize = std::make_shared<BRModeFSM::Synchronize>();
					const auto gameLoop = std::make_shared<BRModeFSM::GameLoop>();
					const auto matchEnd = std::make_shared<BRModeFSM::MatchEnd>();
					const auto reset = std::make_shared<BRModeFSM::Reset>();
					
					return builder.WithState(init)
								  .WithState(countdown)
								  .WithState(synchronize)
								  .WithState(gameLoop)
								  .WithState(matchEnd)
								  .WithState(reset)
	                              .WithTransition(std::make_unique<BRModeFSM::EnterCountdown>(init, countdown))
	                              .WithTransition(std::make_unique<BRModeFSM::EnterSynchronize>(countdown, synchronize))
	                              .WithTransition(std::make_unique<BRModeFSM::EnterGameLoop>(synchronize, gameLoop))
								  .WithTransition(std::make_unique<BRModeFSM::EnterMatchEnd>(gameLoop, matchEnd))
								  .WithTransition(std::make_unique<BRModeFSM::EnterReset>(matchEnd, reset))
	                              .WithInitialState(init->GetID())
	                              .Build(context);
				}
			
			default:
				checkf(false, TEXT("States Machine type %d not defined"), type);
				return {};
		}	
	}
	std::unique_ptr<StatesMachineFactory::PlayerStateFSM> StatesMachineFactory::CreatePlayerStateFSM(
			FSMType type, 
			std::shared_ptr<BRPlayerStateFSM::PlayerStateContext> context)
	{
		StatesMachineBuilder<BRPlayerStateFSM::PlayerStateState, BRPlayerStateFSM::PlayerStateContext> builder;

		switch(type)
		{
		case FSMType::PLAYER_STATE_SERVER:
			{
				const auto init = std::make_shared<BRPlayerStateFSM::Init>();
				const auto gameLoop = std::make_shared<BRPlayerStateFSM::GameLoop>();
				const auto dead = std::make_shared<BRPlayerStateFSM::ServerDead>();
				const auto victory = std::make_shared<BRPlayerStateFSM::ServerVictory>();
				const auto gameOver = std::make_shared<BRPlayerStateFSM::ServerGameOver>();
					
				return builder.WithState(init)
							  .WithState(gameLoop)
							  .WithState(dead)
							  .WithState(victory)
							  .WithState(gameOver)
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterGameLoop>(init, gameLoop))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterDead>(gameLoop, dead))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterVictory>(gameLoop, victory))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::ServerEnterGameOver>(dead, gameOver))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::ServerEnterGameOver>(victory, gameOver))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::ServerEnterInit>(gameOver, init))
							  .WithInitialState(init->GetID())
							  .Build(context);
			}
		case FSMType::PLAYER_STATE_CLIENT:
			{
				const auto init = std::make_shared<BRPlayerStateFSM::Init>();
				const auto gameLoop = std::make_shared<BRPlayerStateFSM::GameLoop>();
				const auto dead = std::make_shared<BRPlayerStateFSM::ClientDead>();
				const auto victory = std::make_shared<BRPlayerStateFSM::ClientVictory>();
				const auto gameOver = std::make_shared<BRPlayerStateFSM::ClientGameOver>();
					
				return builder.WithState(init)
							  .WithState(gameLoop)
							  .WithState(dead)
							  .WithState(victory)
							  .WithState(gameOver)
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterGameLoop>(init, gameLoop))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterDead>(gameLoop, dead))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::EnterVictory>(gameLoop, victory))
							  .WithTransition(std::make_unique<BRPlayerStateFSM::ClientEnterInit>(gameOver, init))
							  .WithInitialState(init->GetID())
							  .Build(context);
			}
		default:
			checkf(false, TEXT("States Machine type %d not defined"), type);
			return {};
		}	
	}
}
