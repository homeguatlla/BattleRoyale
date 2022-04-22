#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/Tests/GameRules/MockClasses.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameRulesTest_When_AddingARuleTwice_Then_IsNotAdded,
								 "Project.GameRules.When_AddingARuleTwice_Then_IsNotAdded",
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
								 "Project.GameRules.When_AddingTwoEqualsRules_Then_TheSecondOneIsNotAdded",
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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameRulesTest_When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAdded,
                                 "Project.GameRules.When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAdded",
                                 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FGameRulesTest_When_SomeoneDieAndIsTheLastOne_Then_EndOfGameRuleIsAdded::RunTest(const FString& Parameters)
{
	const auto gameState = NewObject<GameStateMock>();

	gameState->Initialize(2, 2);

	//Eliminate one character so 1 team is alive
	const auto character = gameState->GetCharacter(0);
	character->SetCurrentHealth(0.0f);
	
	const auto rule = std::make_shared<CheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(gameState);
	
	GameRules rules;

	rules.AddRule(rule);

	TestTrue(TEXT("Num rules before execute "), rules.GetNumRules() == 1);

	//Checkthereisonlyoneteamaliverule will be removed and endofgame will be added
	rules.Execute();

	TestTrue(TEXT("When last team new rule is added"), rules.GetNumRules() == 1);

	return true;
}

#endif
#endif
