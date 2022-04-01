// Fill out your copyright notice in the Description page of Project Settings.


#include "GameRules.h"

UGameRules::UGameRules()
{
}

void UGameRules::AddRule(TScriptInterface<IIGameRule> rule)
{
	mRules.Add(rule);
}

void UGameRules::Execute()
{
	bool hasRulesListBeenModified = false;

	TArray<TScriptInterface<IIGameRule>> newRules(mRules);
	
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

	//TODO 1) faltaría volver a evaluar las reglas?
	//TODO 2) si no han habido modificaciones no hace falta revertir la copia
	//esto se podría optimizar para que pillara una de las dos listas cada vez al entrar
	//en la función dependiendo de si han sido modificada o no, haciendo solo una copia en lugar de dos.
}
