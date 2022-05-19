// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "BattleRoyale/core/GameMode/GameRules/IGameRule.h"


class BATTLEROYALE_API TimerRule: public IGameRule
{

public:
	TimerRule() = default;
	virtual ~TimerRule() = default;
	
	virtual void Initialize(UWorld* world, IIGameState* gameState) override;
	virtual bool Evaluate() override;
	virtual bool Execute(std::vector<std::shared_ptr<IGameRule>>& rules) const override;

	void SetDuration(float duration) { mDuration = duration;}
	void Start();
	
private:
	void OnTimerFinished();
	FTimerHandle mTimerHandle;
	bool mIsTimerOver;
	float mDuration;
	
protected:	
	IIGameState* mGameState;
	UWorld* mWorld;
};

