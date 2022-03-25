#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

#include "BattleRoyale/Tests/TestUtils.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSprintTest_WhenSpawningANewCharacter_CanNotSprint,
                                 "Project.Abilities.SprintAbility.When_SpawningNewCharacter_Then_CanNotSprint",
                                 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FSprintTest_WhenSpawningANewCharacter_CanNotSprint::RunTest(const FString& Parameters)
{

	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto character = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), character);

	TestFalse(TEXT("When Spawning New Character Can NOT Sprint"), character->CanSprint());

	character->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSprintTest_WhenCharacterWalkingAndSpeedGreaterThanZero_CanSprint,
								 "Project.Abilities.SprintAbility.When_CharacterWalkingAndSpeedGreaterThanZero_Then_CanSprint",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FSprintTest_WhenCharacterWalkingAndSpeedGreaterThanZero_CanSprint::RunTest(const FString& Parameters)
{
	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto characterBase = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);

	characterBase->GetCharacterMovement()->Velocity = FVector(100, 0, 0);
	characterBase->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);

	const auto character = Cast<IICharacter>(characterBase);
	TestTrue(TEXT("When Character is Walking and Speed > 0 Can Sprint"), character->CanSprint());

	characterBase->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FSprintTest_WhenAbilitySprintIsTriggered_And_CanSprint_CharacterIsSprinting,
								 "Project.Abilities.SprintAbility.When_AbilitySprintIsTriggered_And_CanSprint_Then_CharacterIsSprinting",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FSprintTest_WhenAbilitySprintIsTriggered_And_CanSprint_CharacterIsSprinting::RunTest(const FString& Parameters)
{
	const FString mapName = TEXT("/Game/Maps/SampleTest.SampleTest");
	if (!AutomationOpenMap(mapName))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load world"));
		return false;
	}
 
	UWorld *world = GetAnyGameWorld();
	if (!world)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve world"));
		return false;
	}

	const auto gameState = Cast<IIGameState>(world->GetGameState());
	
	while(!gameState->DidCountdownFinish()){};
	
	const auto playerController = world->GetFirstPlayerController();
	const auto character = UGameplayStatics::GetPlayerCharacter(world, 0);
	if(!character)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve character"));
		return false;
	}
	const auto characterBase = Cast<ACharacterBase>(character);
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);

	const auto characterMovementComponent = characterBase->GetCharacterMovement();
	
	characterMovementComponent->Velocity = FVector(100, 0, 0);
	characterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	const auto maxWalkSpeedBeforeSprint = characterMovementComponent->MaxWalkSpeed;

	//Enable ability simulating key left shift (sprint) pressed.
	const auto playerState = Cast<APlayerStateBase>(characterBase->GetPlayerState());
	const auto gas = playerState->GetAbilitySystemComponent();

	gas->AbilityLocalInputPressed(static_cast<uint8>(EAbilityInputID::Sprint));
	TestTrue(TEXT("When Character is sprinting SPRINT max walk speed increased"),characterMovementComponent->MaxWalkSpeed > maxWalkSpeedBeforeSprint);

	world->DestroyWorld(true);
	
	return true;
}
#endif
#endif