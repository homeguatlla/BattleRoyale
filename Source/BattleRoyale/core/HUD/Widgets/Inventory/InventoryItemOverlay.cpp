// Fill out your copyright notice in the Description page of Project Settings.


#include "InventoryItemOverlay.h"
#include "IInventoryItemOverlay.h"
#include "BattleRoyale/core/Character/CharacterBase.h"

//TODO necesitaremos el item id para poder hacer el drop correctamente y actualizar el inventory como hacer un refresh del inventorybagvisual
void UInventoryItemOverlay::SetItem(int id, int value, const TSubclassOf<UUserWidget>& widget)
{
	if(!widget->IsValidLowLevel())
	{
		return;
	}
	const auto itemWidget = CreateWidget<UUserWidget>(this, widget);
	if(!itemWidget)
	{
		return;
	}

	mID = id;
	CurrentWidgetClass = widget;
	RemoveChildAt(0);
	AddChild(itemWidget);
	
	if(itemWidget->Implements<UIInventoryItemOverlay>())
	{
		const auto inventoryItemOverlayWidget = Cast<IIInventoryItemOverlay>(itemWidget);
		inventoryItemOverlayWidget->Execute_SetValue(itemWidget, value);
	}
	
	mIsEmpty = false;
}

void UInventoryItemOverlay::RemoveItem()
{
	if(mIsEmpty)
	{
		return;
	}
	const auto itemWidget = CreateWidget<UUserWidget>(this, DefaultWidgetClass);
	if(!itemWidget)
	{
		return;
	}
	CurrentWidgetClass = DefaultWidgetClass;
	RemoveChildAt(0);
	AddChild(itemWidget);
	mIsEmpty = true;
	mID = -1;
}

void UInventoryItemOverlay::DropItem()
{
	//TODO igual aquí podríamos tener un efecto visual un escalado o algo.
	GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Green, FString("Drop"));
	const auto playerController = GetOwningPlayer();
	if(!playerController)
	{
		return;
	}
	const auto character = Cast<IICharacter>(playerController->GetCharacter());
	if(!character)
	{
		return;
	}
	const auto inventoryComponent = character->GetInventoryComponent();
	check(inventoryComponent);
	inventoryComponent->OnDropInventoryItem(mID);
	RemoveItem();
}

