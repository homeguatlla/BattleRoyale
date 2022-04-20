// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>
#include <vector>

#include "IGameRule.h"


class BATTLEROYALE_API GameRules
{
public:
	GameRules() = default;

	void AddRule(std::shared_ptr<IGameRule> rule);
	void Execute();

	//Test purposes
	int GetNumRules() const { return mRules.size(); }
	
private:
	UPROPERTY()
	std::vector<std::shared_ptr<IGameRule>> mRules;
};
