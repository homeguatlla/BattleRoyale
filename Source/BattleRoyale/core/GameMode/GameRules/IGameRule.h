// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <vector>
#include <memory>

class IIGameState;

class BATTLEROYALE_API IGameRule
{
public:
	virtual ~IGameRule() = default;
	
	virtual void Initialize(UWorld* world, IIGameState* gameState) = 0;
	virtual bool Evaluate() = 0;
	//return true if the list have been modified
	virtual bool Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const = 0;
};
