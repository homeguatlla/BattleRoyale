// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckThereIsOnlyOneTeamAliveRule.h"

#include "EndOfGameRule.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"
#include "GameFramework/GameModeBase.h"

void UCheckThereIsOnlyOneTeamAliveRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UCheckThereIsOnlyOneTeamAliveRule::Evaluate()
{
	if(mGameState->GetNumPlayers() <= 0)
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

bool UCheckThereIsOnlyOneTeamAliveRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	UE_LOG(LogGameMode, Display, TEXT("GameRules: Executing Rule CheckThereIsOnlyOneTeamAliveRule"));
	
	//Set data 
	mGameState->SetWinnerTeam(mTeamIdAlive);

	//remove rules not needed
	/*TScriptInterface<IIGameRule> thisRule;
	thisRule.SetObject(Cast<UObject>(this));
	
	rules.Remove(thisRule);
*/
	//add new rules
	const auto endOfGameRule = NewObject<UEndOfGameRule>();
	endOfGameRule->Initialize(mGameState);
	rules.Add(endOfGameRule);

	//return true if added/removed rules
	return true;
}
