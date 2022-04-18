#include <vector>

#include "CoreMinimal.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/BattleRoyale/GameRules/CheckThereIsOnlyOneTeamAliveRule.h"
#include "BattleRoyale/core/GameMode/GameRules/GameRules.h"
#include "BattleRoyale/core/Utils/TeamSelectionStrategies/ITeamSelectionStrategy.h"
#include "BattleRoyale/core/Utils/TeamSelectionStrategies/SimpleTeamSelectionStrategy.h"
#include "BattleRoyale/Tests/GameRules/MockClasses.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSimpleTeamSelectionStrategyTest_When_ATeamIsRequestedFor_1PlayerPerTeam_Then_ProperTeamIdIsReturned,
                                 "Project.Utils.TeamSelectionStrategy.SimpleTeamSelectionStrategy.When_ATeamIsRequestedFor_1PlayerPerTeam_Then_ProperTeamIdIsReturned",
                                 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FSimpleTeamSelectionStrategyTest_When_ATeamIsRequestedFor_1PlayerPerTeam_Then_ProperTeamIdIsReturned::RunTest(const FString& Parameters)
{
	const auto strategy = Cast<ITeamSelectionStrategy>(NewObject<USimpleTeamSelectionStrategy>());
	strategy->Initialize(1);

	const std::vector<int> teamIdsExpected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	
	for(auto i = 0 ; i < 10; ++i)
	{
		const auto teamId = strategy->GetNextTeamId();
		auto message = FString::Printf(TEXT("Team id %d with value %d is properly generated (%d) "), i, teamId, teamIdsExpected[i]);
		TestTrue(message, teamId == teamIdsExpected[i]);
	}
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSimpleTeamSelectionStrategyTest_When_ATeamIsRequestedFor_2PlayerPerTeam_Then_ProperTeamIdIsReturned,
								 "Project.Utils.TeamSelectionStrategy.SimpleTeamSelectionStrategy.When_ATeamIsRequestedFor_2PlayerPerTeam_Then_ProperTeamIdIsReturned",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FSimpleTeamSelectionStrategyTest_When_ATeamIsRequestedFor_2PlayerPerTeam_Then_ProperTeamIdIsReturned::RunTest(const FString& Parameters)
{
	const auto strategy = Cast<ITeamSelectionStrategy>(NewObject<USimpleTeamSelectionStrategy>());
	strategy->Initialize(2);

	const std::vector<int> teamIdsExpected = {0, 1, 0, 1, 2, 2, 3, 3, 4, 4};
	
	for(auto i = 0 ; i < 10; ++i)
	{
		const auto teamId = strategy->GetNextTeamId();
		auto message = FString::Printf(TEXT("Team id %d with value %d is properly generated (%d) "), i, teamId, teamIdsExpected[i]);
		TestTrue(message, teamId == teamIdsExpected[i]);
	}
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSimpleTeamSelectionStrategyTest_When_ATeamIsRequestedFor_3PlayerPerTeam_Then_ProperTeamIdIsReturned,
								 "Project.Utils.TeamSelectionStrategy.SimpleTeamSelectionStrategy.When_ATeamIsRequestedFor_3PlayerPerTeam_Then_ProperTeamIdIsReturned",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FSimpleTeamSelectionStrategyTest_When_ATeamIsRequestedFor_3PlayerPerTeam_Then_ProperTeamIdIsReturned::RunTest(const FString& Parameters)
{
	const auto strategy = Cast<ITeamSelectionStrategy>(NewObject<USimpleTeamSelectionStrategy>());
	strategy->Initialize(3);

	const std::vector<int> teamIdsExpected = {0, 1, 0, 1, 0, 1, 2, 2, 2, 3, 3, 3, 4};
	
	for(auto i = 0 ; i < 13; ++i)
	{
		const auto teamId = strategy->GetNextTeamId();
		auto message = FString::Printf(TEXT("Team id %d with value %d is properly generated (%d) "), i, teamId, teamIdsExpected[i]);
		TestTrue(message, teamId == teamIdsExpected[i]);
	}
	
	return true;
}

#endif
#endif
