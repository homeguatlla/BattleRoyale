// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <memory>

#include "BattleRoyale/core/GameMode/GameRules/IGameRule.h"


class BATTLEROYALE_API GameFinishedRule: public IGameRule
{

public:
	GameFinishedRule() = default;
	virtual ~GameFinishedRule() override = default;
	
	virtual void Initialize(UWorld* world, IIGameState* gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const override;

private:
	UWorld* mWorld;
	IIGameState* mGameState;
};