
#include "PlayerStateContext.h"

#include "BattleRoyale/core/Character/ICharacter.h"
#include "BattleRoyale/core/GameMode/IPlayerState.h"

namespace BRPlayerStateFSM
{
	PlayerStateContext::PlayerStateContext(UWorld* world, IIPlayerState* playerState, IICharacter* character, UEventDispatcher* eventDispatcher) :
	mWorld{ world },
	mPlayerState { playerState },
	mCharacter {character },
	mEventDispatcher { eventDispatcher },
	mIsTimerOver {true}
	{
	}

	void PlayerStateContext::StartTimer(float duration)
	{
		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda([this] {
			OnTimeOver();
		});
		mIsTimerOver = false;
		mWorld->GetTimerManager().SetTimer(mTimerHandle, timerDelegate, duration, true);
	}

	void PlayerStateContext::StartTimer(float duration, std::function<void()> callback)
	{
		FTimerDelegate timerDelegate;
		timerDelegate.BindLambda([this, &callback] {
			callback();
			OnTimeOver();
		});
		mIsTimerOver = false;
		mWorld->GetTimerManager().SetTimer(mTimerHandle, timerDelegate, duration, true);
	}

	void PlayerStateContext::StopTimer()
	{
		mIsTimerOver = true;
		mWorld->GetTimerManager().ClearTimer(mTimerHandle);
	}

	void PlayerStateContext::OnTimeOver()
	{
		mIsTimerOver = true;
		StopTimer();
	}
}
