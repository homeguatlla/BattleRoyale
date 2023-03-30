
#pragma once
#include <functional>

#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS


class APlayerStateMock : public APlayerStateBase
{
public:
	APlayerStateMock() = default;

	void Initialize(int teamId)
	{
		SetTeamId(teamId);
	}
};

class UHurtComponentMock : public UHurtComponent
{
	UHurtComponentMock() = default;

	virtual bool IsAlive() { return true; }
};

#endif
#endif
