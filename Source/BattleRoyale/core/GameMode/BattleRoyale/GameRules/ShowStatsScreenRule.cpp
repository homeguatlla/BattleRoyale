// Fill out your copyright notice in the Description page of Project Settings.


#include "ShowStatsScreenRule.h"

#include "CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/BattleRoyale.h"
#include "BattleRoyale/core/GameMode/IGameState.h"

void UShowStatsScreenRule::Initialize(TScriptInterface<IIGameState> gameState)
{
	mGameState = gameState;
}

bool UShowStatsScreenRule::Evaluate()
{
	return Super::Evaluate();
}

bool UShowStatsScreenRule::Execute(TArray<TScriptInterface<IIGameRule>>& rules) const
{
	Super::Execute(rules);
	
	UE_LOG(LogGameRules, Log, TEXT("GameRules: Executing Rule ShowStatsScreenRule"));

	rules.Empty();

	//TODO gamestate show stats screen rule. Porque podríamos enviar el evento aquí pero
	//no se haría multicast y solo lo recibiría el servidor. Pero no me gusta que el gameState
	//tenga un showstatsscreen es un poco raro pero bueno igual está bien.
	return false;
}
