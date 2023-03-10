#include "BattleRoyaleTests.h"

#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Engine.h"
#include "EngineUtils.h"
#include "BattleRoyale/core/Character/CharacterBase.h"

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

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FGameTest, "BattleRoyale.Game",
    EAutomationTestFlags::EditorContext |
    EAutomationTestFlags::ProductFilter)

bool FGameTest::RunTest(const FString& Parameters)
{
    //TODO conseguir que el juego arranque con un solo jugador cambiando el game mode para que soporte un jugador
    //porque parece que sino, se queda esperando... forever Seguramente esperando otro jugador para empezar la partida.
    //Pensar esto bien, porque igual tiene más dificultad que añadir una variable.
    //Es decir, igual hay que crear un nuevo game mode, singleplayer, con su máquina de estados etc. que no pasa por
    //el countdown ni nada por el estilo. Tiene sentido, más que intentar modificar el battleroyale game mode.
    
	const FString mapName = TEXT("/Game/Maps/SampleTest.SampleTest");
    AutomationOpenMap(mapName);

    UWorld* world = GetTestWorld();

    const auto gameMode = world->GetAuthGameMode();
    TestTrue("GameMode class is set correctly",world->GetAuthGameMode()->IsA<ABattleRoyaleGameMode>());

    const auto controller = world->GetFirstPlayerController();
    const auto character = controller->GetPawn();
    //TestTrue(TEXT("Character is spawned"), character != nullptr);

    return true;
}