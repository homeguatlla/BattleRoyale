#pragma once
#include <functional>

class UEventDispatcher;
class IICharacter;

class FSMContextBase
{
public:
	FSMContextBase(UWorld* world);

	virtual ~FSMContextBase() = default;
	
	UWorld* GetWorld() const { return mWorld; }
	void StartTimer(float duration);
	void StartTimer(float duration, std::function<void()> callback);
	void StopTimer();
	bool IsTimeOver() const { return mIsTimerOver; }
	
private:
	void OnTimeOver();
	
	UWorld* mWorld;
	FTimerHandle mTimerHandle;
	bool mIsTimerOver;
};
