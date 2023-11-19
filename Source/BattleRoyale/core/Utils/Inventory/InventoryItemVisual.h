#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryItemVisual.generated.h"

class UInventoryArrayItem;

USTRUCT(BlueprintType)
struct FInventoryItemVisual
{
	GENERATED_BODY()

	FInventoryItemVisual() = default;
	FInventoryItemVisual(int _id, int _value, bool _isEquipped, TSubclassOf<UUserWidget> _widgetClass);
	FInventoryItemVisual(const UInventoryArrayItem* item);

	bool IsEmpty() const;
	void Clear();
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int value = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isEquipped = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UUserWidget> widgetClass = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int id = -1;
};
