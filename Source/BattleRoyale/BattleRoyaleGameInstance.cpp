// Fill out your copyright notice in the Description page of Project Settings.


#include "BattleRoyaleGameInstance.h"

UEventDispatcher* UBattleRoyaleGameInstance::GetEventDispatcher()
{
	return
		IsValid(mEventDispatcher) ?
		mEventDispatcher :
		mEventDispatcher = NewObject<UEventDispatcher>(this, FName("EventDispatcher"));
}
