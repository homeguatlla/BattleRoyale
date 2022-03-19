#include "TestUtils.h"

#if WITH_DEV_AUTOMATION_TESTS

UWorld* GetAnyGameWorld()
{
	UWorld* testWorld = nullptr;
	const TIndirectArray<FWorldContext>& worldContexts = GEngine->GetWorldContexts();
	for (const FWorldContext& context : worldContexts)
	{
		if (((context.WorldType == EWorldType::PIE) || (context.WorldType == EWorldType::Game)) && (context.World()
			!= nullptr))
		{
			testWorld = context.World();
			break;
		}
	}
	return testWorld;
}

#endif