#include "BattleRoyaleTests/BattleRoyaleTests.h"

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "BattleRoyale/core/GameMode/SinglePlayer/SinglePlayerGameMode.h"

//#include "YourGameModeBase.h"
//#include "MyEssentialActor.h"

// Copy of the hidden method GetAnyGameWorld() in AutomationCommon.cpp.
// Marked as temporary there, hence, this one is temporary, too.
UWorld* GetTestWorld() {
    const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
    for (const FWorldContext& Context : WorldContexts) {
        if (((Context.WorldType == EWorldType::PIE) || (Context.WorldType == EWorldType::Game))
            && (Context.World() != nullptr)) {
            return Context.World();
        }
    }

    return nullptr;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameTest, "BattleRoyale.SinglePlayer.Game.LoadMapAndCharacterSpawned",
    EAutomationTestFlags::EditorContext |
    EAutomationTestFlags::ProductFilter)

bool FGameTest::RunTest(const FString& Parameters)
{
	const FString mapName = TEXT("/Game/Maps/SampleTest.SampleTest");
    AutomationOpenMap(mapName);

    UWorld* world = GetTestWorld();

    const auto gameMode = world->GetAuthGameMode();
    TestTrue("GameMode class is set correctly",world->GetAuthGameMode()->IsA<ASinglePlayerGameMode>());

    const auto controller = world->GetFirstPlayerController();
    const auto character = controller->GetPawn();
    TestTrue(TEXT("Character is spawned"), character != nullptr);

    return true;
}