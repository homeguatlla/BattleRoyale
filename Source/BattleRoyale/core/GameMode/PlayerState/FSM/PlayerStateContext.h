#pragma once
#include <functional>

class IICharacter;
class IIPlayerState;

namespace BRPlayerStateFSM
{
	class PlayerStateContext
	{
	public:
		PlayerStateContext(UWorld* world, IIPlayerState* playerState, IICharacter* character);

		~PlayerStateContext() = default;
		
		UWorld* GetWorld() const { return mWorld; }
		IIPlayerState* GetPlayerState() const { return mPlayerState; }
		IICharacter* GetCharacter() const { return mCharacter; }
		void StartTimer(float duration);
		void StartTimer(float duration, std::function<void()> callback);
		void StopTimer();
		bool IsTimeOver() const { return mIsTimerOver; }
		
	private:
		void OnTimeOver();
		
		UWorld* mWorld;
		IIPlayerState* mPlayerState;
		IICharacter* mCharacter;
		
		FTimerHandle mTimerHandle;
		bool mIsTimerOver;
	};
};
