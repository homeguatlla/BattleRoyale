// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "BattleRoyale/core/GameMode/GameRules/IGameRule.h"


class BATTLEROYALE_API EndOfGameRule: public IGameRule
{

public:
	EndOfGameRule() = default;
	virtual ~EndOfGameRule() = default;
	
	virtual void Initialize(IIGameState* gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const override;

private:
	IIGameState* mGameState;
};
