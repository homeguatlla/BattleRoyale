#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/EndOfGameRule.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/Tests/GameRules/MockClasses.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_NoPlayers_Then_EvaluatesFalse,
	"Project.GameRules.Rule.CheckThereIsOnlyOneTeamAlive.When_NoPlayers_Then_EvaluatesFalse",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_NoPlayers_Then_EvaluatesFalse::RunTest(
	const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();
	
	auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(nullptr, gameState);
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
	
	auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(nullptr, gameState);
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
	character->SetCurrentHealthTest(0.0f);
	character = gameState->GetCharacter(2);
	character->SetCurrentHealthTest(0.0f);
	
	auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(nullptr, gameState);
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
	character->SetCurrentHealthTest(0.0f);
	
	auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(nullptr, gameState);
	const auto result = rule->Evaluate();

	TestFalse(TEXT("When one player died but there are two alive Then there is NOT one team alive."), result);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(
	FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_Executed_Then_EndOfGameRule_IsAdded,
	"Project.GameRules.Rule.CheckThereIsOnlyOneTeamAlive.When_Executed_Then_EndOfGameRule_IsAdded",
	EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_CheckThereIsOnlyOneTeamAlive_When_Executed_Then_EndOfGameRule_IsAdded::RunTest(
	const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();

	gameState->Initialize(3, 3);

	//Eliminate one character so 2 teams are alive
	auto character = gameState->GetCharacter(0);
	character->SetCurrentHealthTest(0.0f);
	
	auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(nullptr, gameState);
	
	std::vector<std::shared_ptr<IGameRule>> rules;
	rule->Execute(rules);

	auto s1 = typeid(*rules[0]).name();
	auto s2 = typeid(EndOfGameRule).name();
	const auto isEndOfGameRule = typeid(*rules[0]).name() == typeid(EndOfGameRule).name();
	TestTrue(TEXT("When executed EndOfGameRule is added."), isEndOfGameRule);

	return true;
}
#endif
#endif
