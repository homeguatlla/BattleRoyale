// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayBlueprintFunctionLibrary.generated.h"

class UInventoryItemStaticData;
/**
 * 
 */
UCLASS()
class BATTLEROYALE_API UGameplayBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="GameplayBlueprintFunctionLibrary")
	static UMaterialInstanceDynamic* CreateAndAssignMaterialInstanceDynamicToMeshFromActor(AActor* actor);

	UFUNCTION(BlueprintCallable, Category="GameplayBlueprintFunctionLibrary")
	static UMaterialInstanceDynamic* CreateAndAssignMaterialInstanceDynamicToMesh(USkeletalMeshComponent* meshComponent);
	
	UFUNCTION(BlueprintCallable, Category="GameplayBlueprintFunctionLibrary")
	static void SetMaterialColor(UMaterialInstanceDynamic* dynamicMaterial, const FName& parameterName, const FColor& color);

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const UInventoryItemStaticData* GetInventoryItemStaticData(TSubclassOf<UInventoryItemStaticData> inventoryItemStaticDataClass); 
};
