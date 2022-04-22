// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckThereIsOnlyOneTeamAliveRule.h"

#include "EndOfGameRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"

void CheckThereIsOnlyOneTeamAliveRule::Initialize(UWorld* world, IIGameState* gameState)
{
	mWorld = world;
	mGameState = gameState;
}

bool CheckThereIsOnlyOneTeamAliveRule::Evaluate()
{
	if(mGameState->GetNumPlayers() <= 0)
	{
		return false;
	}

	if(!mGameState->AreAllPlayersReplicated())
	{
		return false;
	}
	
	bool thereIsOnlyOneTeamAlive = true;
	mTeamIdAlive = -1;
	
	mGameState->PerformActionForEachPlayerState(
		[this, &thereIsOnlyOneTeamAlive](const IIPlayerState* playerState)
		{
			if(playerState->IsAlive())
			{
				if(mTeamIdAlive >=0 && mTeamIdAlive != playerState->GetTeamId())
				{
					thereIsOnlyOneTeamAlive = false;
					return true; //finish the search
				}
				else
				{
					mTeamIdAlive = playerState->GetTeamId();
				}
			}
			return false;//continue searching
		});
	
	return thereIsOnlyOneTeamAlive;
}

bool CheckThereIsOnlyOneTeamAliveRule::Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const
{
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule CheckThereIsOnlyOneTeamAliveRule"));
	
	//Set data 
	mGameState->SetWinnerTeam(mTeamIdAlive);

	//remove rules not needed
	/*TScriptInterface<IIGameRule> thisRule;
	thisRule.SetObject(Cast<UObject>(this));
	
	rules.Remove(thisRule);
*/

	rules.clear();
	
	//add new rules
	const auto endOfGameRule = std::make_shared<EndOfGameRule>();
	endOfGameRule->Initialize(mWorld, mGameState);
	rules.push_back(endOfGameRule);

	//return true if added/removed rules
	return true;
}
