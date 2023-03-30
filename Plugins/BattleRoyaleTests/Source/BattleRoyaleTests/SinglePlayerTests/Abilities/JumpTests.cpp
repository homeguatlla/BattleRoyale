#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

#include "BattleRoyaleTests/TestUtils.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

#if WITH_DEV_AUTOMATION_TESTS


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FJumpTest_WhenSpawningANewCharacter_CanJump,
                                 "BattleRoyale.SinglePlayer.Abilities.JumpAbility.When_SpawningNewCharacter_Then_CanJump",
                                 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FJumpTest_WhenSpawningANewCharacter_CanJump::RunTest(const FString& Parameters)
{
	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto characterBase = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);
	characterBase->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	const auto character = Cast<IICharacter>(characterBase);

	TestTrue(TEXT("When Spawning New Character Can Jump"), character->CanJump());

	characterBase->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FJumpTest_WhenCharacterIsFalling_CanNotJump,
								 "BattleRoyale.SinglePlayer.Abilities.JumpAbility.When_CharacterIsFalling_Then_CanNotJump",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FJumpTest_WhenCharacterIsFalling_CanNotJump::RunTest(const FString& Parameters)
{
	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto characterBase = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);

	characterBase->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);

	const auto character = Cast<IICharacter>(characterBase);
	
	TestFalse(TEXT("When Character is Falling Can't Jump"), character->CanJump());

	characterBase->Destroy();

	return true;
}


DEFINE_LATENT_AUTOMATION_COMMAND_ONE_PARAMETER(FMyCommand, IIGameState*, GameState);
bool FMyCommand::Update() {
	
	if(GameState->IsGameReadyToStart())
	{
		return true;
	}
	else
	{
		return false;
	}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FJumpTest_WhenAbilityJumpIsTriggered_And_CanJump_CharacterIsJumping,
								 "BattleRoyale.SinglePlayer.Abilities.JumpAbility.When_AbilityJumpIsTriggered_And_CanJump_Then_CharacterIsJumping",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)
PRAGMA_DISABLE_OPTIMIZATION_ACTUAL
bool FJumpTest_WhenAbilityJumpIsTriggered_And_CanJump_CharacterIsJumping::RunTest(const FString& Parameters)
{
	/*ADD_LATENT_AUTOMATION_COMMAND(FEditorAutomationLogCommand("Hola test"));
	ADD_LATENT_AUTOMATION_COMMAND(FWaitLatentCommand(5.0));
	*/
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
	
	//ADD_LATENT_AUTOMATION_COMMAND(FMyCommand(gameState));
	//ADD_LATENT_AUTOMATION_COMMAND(FEngineWaitLatentCommand(5.0f));
	//while(!gameState->IsGameReadyToStart()){};
	
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
	characterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	const auto originalVelocityZ = characterBase->GetVelocity().Z;
	
	//Enable ability simulating key left shift (sprint) pressed.
	const auto playerState = Cast<APlayerStateBase>(characterBase->GetPlayerState());
	const auto gas = playerState->GetAbilitySystemComponent();

	gas->AbilityLocalInputPressed(static_cast<uint8>(EAbilityInputID::Jump));

	characterBase->GetCharacterMovement()->DoJump(false);
	
	const auto newVelocityZ = characterBase->GetVelocity().Z;
	TestTrue(TEXT("When Character is jumping Z is increased"), newVelocityZ > originalVelocityZ);
	
	//world->DestroyWorld(false);
	
	return true;
}
PRAGMA_ENABLE_OPTIMIZATION_ACTUAL
#endif
#endif