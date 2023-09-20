#include "InventoryBagMock.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryArrayItem.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryBagVisual.h"
#include "BattleRoyale/core/Utils/Inventory/InventoryItemVisual.h"
#if WITH_EDITOR
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryBagVisualTests_When_RefreshWithEmptyInventoryBag_Then_InventoryBagVisualIsEmpty,
								 "BattleRoyale.Singleplayer.Inventory.InventoryBagVisual.When_RefreshWithEmptyInventoryBag_Then_InventoryBagVisualIsEmpty",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInventoryBagVisualTests_When_RefreshWithEmptyInventoryBag_Then_InventoryBagVisualIsEmpty::RunTest(const FString& Parameters)
{
	const auto inventoryBag = NewObject<UInventoryBagMock>();

	TestTrue("InventoryBag empty", inventoryBag->IsEmpty());

	const auto inventoryBagVisual = NewObject<UInventoryBagVisual>();
	inventoryBagVisual->Refresh(inventoryBag);

	TestTrue("InventoryBagVisual empty", inventoryBagVisual->IsEmpty());

	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryBagVisualTests_When_RefreshWithNotEmptyInventoryBag_Then_InventoryBagVisualHasTheSameNumberOfElements,
								 "BattleRoyale.Singleplayer.Inventory.InventoryBagVisual.When_RefreshWithNotEmptyInventoryBag_Then_InventoryBagVisualHasTheSameNumberOfElements",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInventoryBagVisualTests_When_RefreshWithNotEmptyInventoryBag_Then_InventoryBagVisualHasTheSameNumberOfElements::RunTest(const FString& Parameters)
{
	const auto inventoryBag = NewObject<UInventoryBagMock>();

	inventoryBag->AddItem(InventoryItemMock(0, 0));
	inventoryBag->AddItem(InventoryItemMock(1, 1));
	inventoryBag->AddItem(InventoryItemMock(2, 1));

	TestTrue("InventoryBag has 3 elements", inventoryBag->Num() == 3);
	
	const auto inventoryBagVisual = NewObject<UInventoryBagVisual>();
	inventoryBagVisual->Refresh(inventoryBag);

	TestTrue("InventoryBagVisual has 3 elements", inventoryBagVisual->Num() == 3);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryBagVisualTests_When_RefreshAfterRemovingElementsInTheInventoryBag_Then_InventoryBagVisualRemovesThoseElements,
								 "BattleRoyale.Singleplayer.Inventory.InventoryBagVisual.When_RefreshAfterRemovingElementsInTheInventoryBag_Then_InventoryBagVisualRemovesThoseElements",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInventoryBagVisualTests_When_RefreshAfterRemovingElementsInTheInventoryBag_Then_InventoryBagVisualRemovesThoseElements::RunTest(const FString& Parameters)
{
	const auto inventoryBag = NewObject<UInventoryBagMock>();

	inventoryBag->AddItem(InventoryItemMock(0, 0));
	inventoryBag->AddItem(InventoryItemMock(1, 1));
	inventoryBag->AddItem(InventoryItemMock(2, 1));

	TestTrue("InventoryBag has 3 elements", inventoryBag->Num() == 3);
	
	const auto inventoryBagVisual = NewObject<UInventoryBagVisual>();
	inventoryBagVisual->Refresh(inventoryBag);
	TestTrue("InventoryBagVisual has 3 elements", inventoryBagVisual->Num() == 3);

	inventoryBag->RemoveItem(1);
	TestTrue("After removing one element InventoryBag has 2 elements", inventoryBag->Num() == 2);
	
	inventoryBagVisual->Refresh(inventoryBag);
	TestTrue("InventoryBagVisual has 2 elements", inventoryBagVisual->Num() == 2);
	
	return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FInventoryBagVisualTests_When_GivenAInventoryBagVisualAndAReorderedInventoryBag_Then_InventoryVisualBagPreservesTheOrder,
								 "BattleRoyale.Singleplayer.Inventory.InventoryBagVisual.When_GivenAInventoryBagVisualAndAReorderedInventoryBag_Then_InventoryVisualBagPreservesTheOrder",
								 EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::ProductFilter)

bool FInventoryBagVisualTests_When_GivenAInventoryBagVisualAndAReorderedInventoryBag_Then_InventoryVisualBagPreservesTheOrder::RunTest(const FString& Parameters)
{
	const auto inventoryBag = NewObject<UInventoryBagMock>();

	inventoryBag->AddItem(InventoryItemMock(0, 0));
	inventoryBag->AddItem(InventoryItemMock(1, 1));
	inventoryBag->AddItem(InventoryItemMock(2, 1));
	
	const auto inventoryBagVisual = NewObject<UInventoryBagVisual>();
	inventoryBagVisual->Refresh(inventoryBag);
	TestTrue("Initial state InventoryBagVisual has 3 elements", inventoryBagVisual->Num() == 3);


	inventoryBag->RemoveItem(0);
	inventoryBag->AddItem(InventoryItemMock(0, 0));

	int i = 0;
	int order[3] = {1, 2, 0};
	inventoryBag->PerformActionForEachItem([this, &i, &order](const FInventoryArrayItem& item)-> bool
	{
		TestTrue("InventoryBag element", item.GetID() == order[i]);
		++i;
		return false;
	});
	
	inventoryBagVisual->Refresh(inventoryBag);

	TestTrue("InventoryBagVisual element", inventoryBagVisual->GetItemByIndex(0).id == 0);
	TestTrue("InventoryBagVisual element", inventoryBagVisual->GetItemByIndex(1).id == 1);
	TestTrue("InventoryBagVisual element", inventoryBagVisual->GetItemByIndex(2).id == 2);
	
	return true;
}

//Añadir uno para cuando se cambia el valor de un elemento, que se cambie también en el visual

//Añadir uno de swap por ids (otra operación)



#endif
#endif