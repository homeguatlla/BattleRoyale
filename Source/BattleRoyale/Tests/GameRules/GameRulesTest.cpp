#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/Tests/GameRules/MockClasses.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS

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
	
	TScriptInterface<IIGameState> gameStateInterface;
	gameStateInterface.SetObject(gameState);
	gameStateInterface.SetInterface(Cast<IIGameState>(gameState));

	UCheckThereIsOnlyOneTeamAliveRule* rule = NewObject<UCheckThereIsOnlyOneTeamAliveRule>();
	rule->Initialize(gameStateInterface);
	
	const auto rules = NewObject<UGameRules>();

	rules->AddRule(rule);

	TestTrue(TEXT("Num rules before execute "), rules->GetNumRules() == 1);

	rules->Execute();

	TestTrue(TEXT("When last team new rule is added"), rules->GetNumRules() == 2);

	return true;
}

#endif
#endif
