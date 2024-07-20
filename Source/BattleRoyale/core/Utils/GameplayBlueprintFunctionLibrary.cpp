// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayBlueprintFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Inventory/InventoryItemStaticData.h"


UMaterialInstanceDynamic* UGameplayBlueprintFunctionLibrary::CreateAndAssignMaterialInstanceDynamicToMeshFromActor(AActor* actor)
{
	UMaterialInstanceDynamic* dynamicMaterial = nullptr;

	if(actor != nullptr)
	{
		const auto meshComponent = actor->FindComponentByClass<USkeletalMeshComponent>();
		if(meshComponent != nullptr)
		{ 
			return CreateAndAssignMaterialInstanceDynamicToMesh(meshComponent);
		}
	}

	check(dynamicMaterial != nullptr);

	return dynamicMaterial;
}

UMaterialInstanceDynamic* UGameplayBlueprintFunctionLibrary::CreateAndAssignMaterialInstanceDynamicToMesh(USkeletalMeshComponent* meshComponent)
{
	UMaterialInstanceDynamic* dynamicMaterial = nullptr;

	if(meshComponent != nullptr)
	{ 
		const auto material = meshComponent->GetMaterial(0);
		if(material != nullptr)
		{ 
			dynamicMaterial = UMaterialInstanceDynamic::Create(material, NULL);
			if(dynamicMaterial != nullptr)
			{
				meshComponent->SetMaterial(0, dynamicMaterial);
			}
		}
	}
	
	check(dynamicMaterial != nullptr);

	return dynamicMaterial;
}

void UGameplayBlueprintFunctionLibrary::SetMaterialColor(UMaterialInstanceDynamic* dynamicMaterial, const FName& parameterName, const FColor& color)
{
	dynamicMaterial->SetVectorParameterValue(parameterName, color);
}

const UInventoryItemStaticData* UGameplayBlueprintFunctionLibrary::GetInventoryItemStaticData(
	TSubclassOf<UInventoryItemStaticData> inventoryItemStaticDataClass)
{
	if(!IsValid(inventoryItemStaticDataClass))
		return nullptr;

	return GetDefault<UInventoryItemStaticData>(inventoryItemStaticDataClass);
}

FVector UGameplayBlueprintFunctionLibrary::GenerateRandomPointAtDistanceOf(const FVector& origin, float distance, bool upSemiSector)
{
	const auto radius = FMath::RandRange(0.0f, distance);
	const auto u = FMath::RandRange(0.0f, PI * 2.0f);
	auto minorVRange = upSemiSector ? 0.0f : -PI * 0.5f;
	const auto v = FMath::RandRange(minorVRange, PI * 0.5f);

	return origin + FVector(
		radius * FMath::Cos(v) * FMath::Cos(u),
		radius * FMath::Cos(v) * FMath::Sin(u),
		radius * FMath::Sin(v));
}
