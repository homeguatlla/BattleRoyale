#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AmmoMock.h"
#include "WeaponMock.h"
#include "AssetRegistry/AssetRegistryModule.h"

#include "BattleRoyale/core/Abilities/AbilitiesInput.h"
#include "BattleRoyale/core/Character/CharacterBase.h"
#include "BattleRoyale/core/GameMode/PlayerState/PlayerStateBase.h"
#include "BattleRoyale/core/PickableObjects/Weapons/IWeapon.h"
#include "BattleRoyale/core/PickableObjects/Weapons/WeaponBase.h"

#include "BattleRoyaleTests/TestUtils.h"
#include "Engine/ObjectLibrary.h"
#include "Kismet/GameplayStatics.h"

#if WITH_EDITOR
#include "Misc/AutomationTest.h"
#include "Tests/AutomationCommon.h"
#include "Tests/AutomationEditorCommon.h"

#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReloadTest_When_Spawn_Then_CanNotReload,
                                 "BattleRoyale.SinglePlayer.Abilities.ReloadAbility.When_Spawn_Then_CanNotReload",
                                 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FReloadTest_When_Spawn_Then_CanNotReload::RunTest(const FString& Parameters)
{

	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto character = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), character);

	const auto gunComponent = character->GetGunComponent();
	const auto inventoryComponent = character->GetInventoryComponent();
	
	TestFalse(TEXT("When Spawning New Character Can NOT Reload"), gunComponent->CanReload(inventoryComponent));

	character->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReloadTest_When_NoWeapon_Then_CanNotReload,
								 "BattleRoyale.SinglePlayer.Abilities.ReloadAbility.When_NoWeapon_Then_CanNotReload",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FReloadTest_When_NoWeapon_Then_CanNotReload::RunTest(const FString& Parameters)
{

	UWorld* world = FAutomationEditorCommonUtils::CreateNewMap(); 
	if (!world)
	{
		return false;
	}
	
	const auto character = world->SpawnActor<ACharacterBase>();
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), character);

	const auto gunComponent = character->GetGunComponent();

	TestFalse(TEXT("Hasn't a weapon equipped"), gunComponent->HasWeaponEquipped());
	
	const auto inventoryComponent = character->GetInventoryComponent();
	
	TestFalse(TEXT("When Spawning New Character Can NOT Reload"), gunComponent->CanReload(inventoryComponent));

	character->Destroy();

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReloadTest_When_WeaponButNoAmmo_Then_CanNotReload,
								 "BattleRoyale.SinglePlayer.Abilities.ReloadAbility.When_WeaponEquippedButNoAmmo_Then_CanNotReload",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FReloadTest_When_WeaponButNoAmmo_Then_CanNotReload::RunTest(const FString& Parameters)
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
	
	const auto character = UGameplayStatics::GetPlayerCharacter(world, 0);
	if(!character)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve character"));
		return false;
	}
	const auto characterBase = Cast<ACharacterBase>(character);
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);
	
	const auto weapon = NewObject<AWeaponMock>();
	weapon->Initialize(EPickupObjectState::Dropped, 0);

	const auto gunComponent = characterBase->GetGunComponent();
	check(gunComponent);
	TestFalse(TEXT("Hasn't a weapon equipped"), gunComponent->HasWeaponEquipped());
	
	const auto inventoryComponent = characterBase->GetInventoryComponent();
	inventoryComponent->PickupObjectServer(weapon);
	
	const auto hasWeaponEquipped = gunComponent->HasWeaponEquipped();
	TestTrue(TEXT("When Weapon equipped"), hasWeaponEquipped);
	if(!hasWeaponEquipped)
	{
		return false;
	}
	
	TestFalse(TEXT("And No Ammo"), gunComponent->GetEquippedWeapon()->HasAmmo());
	
	TestFalse(TEXT("Can not Reload"), gunComponent->CanReload(inventoryComponent));
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FReloadTest_When_WeaponAndAmmo_Then_CanReload,
								 "BattleRoyale.SinglePlayer.Abilities.ReloadAbility.When_WeaponEquippedAndAmmo_Then_CanReload",
								 EAutomationTestFlags::ApplicationContextMask  | EAutomationTestFlags::ProductFilter)

bool FReloadTest_When_WeaponAndAmmo_Then_CanReload::RunTest(const FString& Parameters)
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
	
	const auto character = UGameplayStatics::GetPlayerCharacter(world, 0);
	if(!character)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to retrieve character"));
		return false;
	}
	const auto characterBase = Cast<ACharacterBase>(character);
	
	//For more tests check AutomationTest.h (lines 1347 - 1639)
	TestNotNull<ACharacterBase>(TEXT("Testing if there is a player actor in level"), characterBase);
	
	const auto weapon = NewObject<AWeaponMock>();
	weapon->Initialize(EPickupObjectState::Dropped, 0);

	const auto gunComponent = characterBase->GetGunComponent();
	check(gunComponent);
	TestFalse(TEXT("Hasn't a weapon equipped"), gunComponent->HasWeaponEquipped());
	
	const auto inventoryComponent = characterBase->GetInventoryComponent();
	inventoryComponent->PickupObjectServer(weapon);
	
	const auto hasWeaponEquipped = gunComponent->HasWeaponEquipped();
	TestTrue(TEXT("When Weapon equipped"), hasWeaponEquipped);
	if(!hasWeaponEquipped)
	{
		return false;
	}

	const auto ammo = NewObject<AAmmoMock>();
	ammo->Initialize(EPickupObjectState::Dropped, 10);
	inventoryComponent->PickupObjectServer(ammo);
	
	TestTrue(TEXT("And Ammo"), gunComponent->GetEquippedWeapon()->HasAmmo());
	
	TestTrue(TEXT("Can Reload"), gunComponent->CanReload(inventoryComponent));
	
	return true;
}

#endif
#endif