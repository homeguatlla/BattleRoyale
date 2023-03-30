#include "CoreMinimal.h"
#include "GameStateMock.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyaleTests/MultiplayerTests/GameRules/MockClasses.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameRulesTest_When_AddingARuleTwice_Then_IsNotAdded,
								 "BattleRoyale.Multiplayer.GameRules.When_AddingARuleTwice_Then_IsNotAdded",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_When_AddingARuleTwice_Then_IsNotAdded::RunTest(const FString& Parameters)
{
	const auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	
	GameRules rules;

	rules.AddRule(rule);

	TestTrue(TEXT("New rule is added"), rules.GetNumRules() == 1);

	rules.AddRule(rule);
	
	TestTrue(TEXT("Adding the same rule twice is not added"), rules.GetNumRules() == 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameRulesTest_When_AddingTwoEqualsRules_Then_TheSecondOneIsNotAdded,
								 "BattleRoyale.Multiplayer.GameRules.When_AddingTwoEqualsRules_Then_TheSecondOneIsNotAdded",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_When_AddingTwoEqualsRules_Then_TheSecondOneIsNotAdded::RunTest(const FString& Parameters)
{
	const auto rule1 = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	const auto rule2 = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	
	GameRules rules;

	rules.AddRule(rule1);

	TestTrue(TEXT("New rule is added"), rules.GetNumRules() == 1);

	rules.AddRule(rule2);
	
	TestTrue(TEXT("Adding the same rule twice is not added"), rules.GetNumRules() == 1);

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameRulesTest_When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAddedAndExecuted,
                                 "BattleRoyale.Multiplayer.GameRules.When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAddedAndExecuted",
                                 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAddedAndExecuted::RunTest(const FString& Parameters)
{
	const auto gameState = NewObject<AGameStateMock>();

	gameState->Initialize(2, 2);

	//Eliminate one character so 1 team is alive
	const auto character = gameState->GetCharacter(0);
	character->SetCurrentHealthTest(0.0f);
	
	const auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(nullptr, gameState);
	
	GameRules rules;

	rules.AddRule(rule);

	TestTrue(TEXT("There isn't a winner team"), gameState->GetWinnerTeam() == -1);
	TestTrue(TEXT("Num rules before execute "), rules.GetNumRules() == 1);

	//Checkthereisonlyoneteamaliverule will be removed and endofgame will be added
	rules.Execute();

	TestTrue(TEXT("New rules after execute"), rules.GetNumRules() == 0);
	TestTrue(TEXT("There is a winner team"), gameState->GetWinnerTeam() == 1);
	
	return true;
}

#endif
#endif
