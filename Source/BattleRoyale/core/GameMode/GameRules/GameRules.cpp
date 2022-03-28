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
	//TODO
	//creo que lo ideal sería que al hacer Execute, se le pasara por parámetro la lista de reglas
	//de esta manera podría actualizar la lista añadiendo o quitando reglas.
	//añadir o quitar reglas complica el recorrerlas
	//Pensar los casos con tests y ver por donde tirar.
	for(auto rule : mRules)
	{
		if(rule->Evaluate())
		{
			rule->Execute();
		}
	}
}
