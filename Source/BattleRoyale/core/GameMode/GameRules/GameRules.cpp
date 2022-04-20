// Fill out your copyright notice in the Description page of Project Settings.


#include "GameRules.h"

#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"


void GameRules::AddRule(std::shared_ptr<IGameRule> rule)
{
	auto tt = typeid(rule) == typeid(CheckThereIsOnlyOneTeamAliveRule);
	
	auto it = std::find_if(
		mRules.begin(),
		mRules.end(),
		[&rule](std::shared_ptr<IGameRule> const& gameRule)
		{
			return typeid(*gameRule).name() == typeid(*rule).name();
		});

	if(it == mRules.end())
	{
		mRules.push_back(rule);
	}
	/*
	if(!mRules.ContainsByPredicate(
		[&rule](const TScriptInterface<IIGameRule> object)
		{
			return object.GetObject()->IsA(rule.GetObject()->StaticClass());
		}))
	{
		mRules.Add(rule);
	}*/
}

void GameRules::Execute()
{
	bool hasRulesListBeenModified = false;

	do
	{
		hasRulesListBeenModified = false;
		std::vector<std::shared_ptr<IGameRule>> newRules(mRules);
		
		for(const auto rule : mRules)
		{
			if(rule->Evaluate())
			{
				hasRulesListBeenModified |= rule->Execute(newRules);
			}
		}

		if(hasRulesListBeenModified)
		{
			mRules = newRules;
		}
	} while(hasRulesListBeenModified);
	
	//TODO 1) faltaría volver a evaluar las reglas?
	//La opción sencilla es sí, mientras hayan reglas nuevas se van evaluando.
	//La opción compleja, sería que cada vez que se agrega una regla se evalua. Habría que pensar como.
	
	//TODO 2) si no han habido modificaciones no hace falta revertir la copia
	//esto se podría optimizar para que pillara una de las dos listas cada vez al entrar
	//en la función dependiendo de si han sido modificada o no, haciendo solo una copia en lugar de dos.
}
