#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/BattleRoyaleGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAlive.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"


class ACharacterMock : public ACharacterBase
{
public:
	ACharacterMock() = default;

	void Initialize(float health)
	{
		SetCurrentHealth(health);
	}
};

class APlayerStateMock : public APlayerStateBase
{
public:
	APlayerStateMock() = default;

	void Initialize(int teamId)
	{
		SetTeamId(teamId);
	}
};

class GameStateMock : public ABattleRoyaleGameState
{
public:
	GameStateMock()
	{
	}

	void Initialize(int numPlayers, int numTeams)
	{
		int teamId = 0;

		for (auto player = 0; player < numPlayers; player++)
		{
			const auto playerState = NewObject<APlayerStateMock>();
			playerState->Initialize(teamId);

			const auto character = NewObject<ACharacterMock>();
			character->Initialize(100);

			character->SetPlayerState(playerState);
			
			PlayerArray.Add(playerState);
			teamId = (teamId + 1) % numTeams;
		}
	}

	IICharacter* GetCharacter(int index)
	{
		auto playerState = Cast<APlayerStateMock>(PlayerArray[index]);
		IICharacter* character = Cast<IICharacter>(playerState->GetPawn());

		return character;
	}
	
	virtual bool DidCountdownStart() const override { return false; }
	virtual bool DidCountdownFinish() const override { return false; }

	virtual void StartCountdownServer(int duration) override
	{
	}

	virtual void
	PerformActionForEachPlayerState(std::function<bool(const IIPlayerState* playerState)> action) const override
	{
		//TODO pensar esto. En teoría creo que lo ideal sería poder agregar player states con datos
		//pero eso es más complicado creo.
		//Aquí deberíamos definir un array de player states y ejecutar las acciones
		//si es posible rellenar le propio player state??
		ABattleRoyaleGameState::PerformActionForEachPlayerState(action);
	}
};

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_NoPlayers_Then_EvaluatesFalse,
	"Project.GameRules.Rule.CheckThereIsOnlyOneTeamAlive.When_NoPlayers_Then_EvaluatesFalse",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_NoPlayers_Then_EvaluatesFalse::RunTest(
	const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();

	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(gameState);
	gameStateInterface.SetInterface(Cast<IIGameState>(gameState));

	UCheckThereIsOnlyOneTeamAlive* rule = NewObject<UCheckThereIsOnlyOneTeamAlive>();
	rule->Initialize(gameStateInterface);
	const auto result = rule->Evaluate();

	TestFalse(TEXT("When no players Then the aren't alive teams."), result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_ThereIsOnePlayer_Then_EvaluatesTrue,
	"Project.GameRules.Rule.CheckThereIsOnlyOneTeamAlive.When_ThereIsOnePlayer_Then_EvaluatesTrue",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_ThereIsOnePlayer_Then_EvaluatesTrue::RunTest(
	const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();

	gameState->Initialize(1, 1);

	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(gameState);
	gameStateInterface.SetInterface(Cast<IIGameState>(gameState));

	UCheckThereIsOnlyOneTeamAlive* rule = NewObject<UCheckThereIsOnlyOneTeamAlive>();
	rule->Initialize(gameStateInterface);
	const auto result = rule->Evaluate();

	TestTrue(TEXT("When one player Then there is one team alive."), result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_SomeoneDieAndIsTheLastOne_Then_EvaluatesTrue,
	"Project.GameRules.Rule.CheckThereIsOnlyOneTeamAlive.When_SomeoneDieAndIsTheLastOne_Then_EvaluatesTrue",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_SomeoneDieAndIsTheLastOne_Then_EvaluatesTrue::RunTest(
	const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();

	gameState->Initialize(3, 3);

	//we eliminate two characters so only one team is alive
	auto character = gameState->GetCharacter(0);
	character->SetCurrentHealth(0.0f);
	character = gameState->GetCharacter(2);
	character->SetCurrentHealth(0.0f);
	
	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(gameState);
	gameStateInterface.SetInterface(Cast<IIGameState>(gameState));

	UCheckThereIsOnlyOneTeamAlive* rule = NewObject<UCheckThereIsOnlyOneTeamAlive>();
	rule->Initialize(gameStateInterface);
	const auto result = rule->Evaluate();

	TestTrue(TEXT("When all minus one player died Then there is one team alive."), result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_SomeoneDieAndIsNotTheLastOne_Then_EvaluatesFalse,
	"Project.GameRules.Rule.CheckThereIsOnlyOneTeamAlive.When_SomeoneDieAndIsNotTheLastOne_Then_EvaluatesFalse",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_SomeoneDieAndIsNotTheLastOne_Then_EvaluatesFalse::RunTest(
	const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();

	gameState->Initialize(3, 3);

	//Eliminate one character so 2 teams are alive
	auto character = gameState->GetCharacter(0);
	character->SetCurrentHealth(0.0f);
	
	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(gameState);
	gameStateInterface.SetInterface(Cast<IIGameState>(gameState));

	UCheckThereIsOnlyOneTeamAlive* rule = NewObject<UCheckThereIsOnlyOneTeamAlive>();
	rule->Initialize(gameStateInterface);
	const auto result = rule->Evaluate();

	TestFalse(TEXT("When one player died but there are two alive Then there is NOT one team alive."), result);

	return true;
}


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameRulesTest_When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAdded,
                                 "Project.GameRules.When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAdded",
                                 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAdded::RunTest(const FString& Parameters)
{
	UGameRules rules;

	rules.AddRule(NewObject<UCheckThereIsOnlyOneTeamAlive>());
	rules.Execute();

	//	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), character);

	//	TestFalse(TEXT("When Spawning New Character Can NOT Sprint"), character->CanSprint());


	return true;
}

#endif
#endif
