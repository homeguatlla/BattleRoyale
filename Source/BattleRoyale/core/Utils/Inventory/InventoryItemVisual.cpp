#include "InventoryItemVisual.h"

#include "InventoryArrayItem.h"
#include "InventoryItemStaticData.h"

FInventoryItemVisual::FInventoryItemVisual(int _id, int _value, bool _isEquipped, TSubclassOf<UUserWidget> _widgetClass)
{
	id = _id;
	value = _value;
	isEquipped = _isEquipped;
	widgetClass = _widgetClass;	
}

FInventoryItemVisual::FInventoryItemVisual(const UInventoryArrayItem* item)
{
	id = item->GetID();
	value = item->mInventoryItem->GetValue1();
	if(item->mInventoryItem)
	{
		widgetClass = item->mInventoryItem->GetStaticData()->GetItemWidgetClass();
	}
	//isEquipped =
}

bool FInventoryItemVisual::IsEmpty() const
{
	return id == -1;
}
	
void FInventoryItemVisual::Clear()
{
	widgetClass = nullptr;
	id = -1;
	value = 0;
	isEquipped = false;
}