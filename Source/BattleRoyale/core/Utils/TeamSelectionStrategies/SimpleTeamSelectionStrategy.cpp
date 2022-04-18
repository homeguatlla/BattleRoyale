// Fill out your copyright notice in the Description page of Project Settings.


#include "SimpleTeamSelectionStrategy.h"

void USimpleTeamSelectionStrategy::Initialize(int numPlayersPerTeam)
{
	mNumPlayersPerTeam = numPlayersPerTeam;
	mLastSequenceIndexGenerated = 0;
}

int USimpleTeamSelectionStrategy::GetNextTeamId()
{
	int nextTeamId = -1;
	
	if(mLastSequenceIndexGenerated < 2 * mNumPlayersPerTeam)
	{
		//case sequence 1 team member: 0, 1
		//case sequence 2 team member: 0, 1, 0, 1
		//case sequence 3 team member: 0, 1, 0, 1, 0, 1
		nextTeamId = mLastSequenceIndexGenerated % 2;
		mLastSequenceIndexGenerated++;
	}
	else
	{
		//case sequence 1 team member: 2, 3, 4, 5
		//case sequence 2 team member: 2, 2, 3, 3, 4, 4, 5, 5
		//case sequence 3 team member: 2, 2, 2, 3, 3, 3, 4, 4, 4, 5, 5, 5

		nextTeamId = mLastSequenceIndexGenerated / mNumPlayersPerTeam;
		mLastSequenceIndexGenerated++;
	}
	
	return nextTeamId;
}
