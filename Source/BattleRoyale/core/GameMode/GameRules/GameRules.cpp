// Fill out your copyright notice in the Description page of Project Settings.


#include "GameRules.h"

UGameRules::UGameRules()
{
}

void UGameRules::AddRule(TScriptInterface<IIGameRule> rule)
{
	//TODO verificar que si la regla ya existe no la inserte de nuevo!
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
	//La opción sencilla es sí, mientras hayan reglas nuevas se van evaluando.
	//La opción compleja, sería que cada vez que se agrega una regla se evalua. Habría que pensar como.
	
	//TODO 2) si no han habido modificaciones no hace falta revertir la copia
	//esto se podría optimizar para que pillara una de las dos listas cada vez al entrar
	//en la función dependiendo de si han sido modificada o no, haciendo solo una copia en lugar de dos.
}
