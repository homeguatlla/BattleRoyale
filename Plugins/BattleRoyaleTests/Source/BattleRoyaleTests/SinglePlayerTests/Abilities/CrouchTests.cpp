#include "CoreMinimal.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/IGameState.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"

#include "BattleRoyaleTests/TestUtils.h"
#include "Components/CapsuleComponent.h"

#include "GameFramework/GameStateBase.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

#if WITH_DEV_AUTOMATION_TESTS


IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCrouchTest_WhenSpawningANewCharacter_CanCrouch,
                                 "BattleRoyale.SinglePlayer.Abilities.CrouchAbility.When_SpawningNewCharacter_Then_CanCrouch",
                                 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FCrouchTest_WhenSpawningANewCharacter_CanCrouch::RunTest(const FString& Parameters)
{
	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto characterBase = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);
	//characterBase->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
	
	const auto character = Cast<IICharacter>(characterBase);
	
	characterBase->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	TestTrue(TEXT("When Spawning New Character Can Crouch"), character->CanCrouch_());

	characterBase->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FCrouchTest_WhenAbilityCrouchIsTriggered_And_CanCrouch_CharacterIsCrouching,
								 "BattleRoyale.SinglePlayer.Abilities.CrouchAbility.WhenAbilityCrouchIsTriggered_And_CanCrouch_CharacterIsCrouching",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)
PRAGMA_DISABLE_OPTIMIZATION_ACTUAL
bool FCrouchTest_WhenAbilityCrouchIsTriggered_And_CanCrouch_CharacterIsCrouching::RunTest(const FString& Parameters)
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

	characterBase->GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;
	
	const auto characterMovementComponent = characterBase->GetCharacterMovement();
	characterMovementComponent->SetMovementMode(EMovementMode::MOVE_Walking);
	const auto originalColliderHalfHeight = characterBase->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	
	//Enable ability simulating key left shift (sprint) pressed.
	const auto playerState = Cast<APlayerStateBase>(characterBase->GetPlayerState());

	characterBase->OnInputActionCrouch();
	//gas->AbilityLocalInputPressed(static_cast<int32>(EAbilityInputID::Jump));

	characterBase->GetCharacterMovement()->Crouch(false);
	
	const auto newColliderHalfHeight = characterBase->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	TestTrue(TEXT("When Character is crouching collider is shorter"), newColliderHalfHeight < originalColliderHalfHeight);
	
	//world->DestroyWorld(false);
	
	return true;
}
PRAGMA_ENABLE_OPTIMIZATION_ACTUAL
#endif
#endif