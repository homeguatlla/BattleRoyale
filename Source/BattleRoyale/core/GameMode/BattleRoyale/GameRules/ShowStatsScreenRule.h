// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleRoyale/core/GameMode/GameRules/TimerRule.h"


class BATTLEROYALE_API ShowStatsScreenRule: public TimerRule
{
public:
	ShowStatsScreenRule() = default;
	virtual ~ShowStatsScreenRule() = default;
	
	virtual void Initialize(IIGameState* gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const override;

private:
	IIGameState* mGameState;
};
