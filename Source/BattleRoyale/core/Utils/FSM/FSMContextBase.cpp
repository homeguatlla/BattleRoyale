
#include "FSMContextBase.h"

FSMContextBase::FSMContextBase(UWorld* world) :
mWorld{ world },
mIsTimerOver {true}
{
}

void FSMContextBase::StartTimer(float duration)
{
	FTimerDelegate timerDelegate;
	timerDelegate.BindLambda([this] {
		OnTimeOver();
	});
	mIsTimerOver = false;
	mWorld->GetTimerManager().SetTimer(mTimerHandle, timerDelegate, duration, true);
}

void FSMContextBase::StartTimer(float duration, std::function<void()> callback)
{
	FTimerDelegate timerDelegate;
	timerDelegate.BindLambda([this, &callback] {
		callback();
		OnTimeOver();
	});
	mIsTimerOver = false;
	mWorld->GetTimerManager().SetTimer(mTimerHandle, timerDelegate, duration, true);
}

void FSMContextBase::StopTimer()
{
	mIsTimerOver = true;
	mWorld->GetTimerManager().ClearTimer(mTimerHandle);
}

void FSMContextBase::OnTimeOver()
{
	mIsTimerOver = true;
	StopTimer();
}

