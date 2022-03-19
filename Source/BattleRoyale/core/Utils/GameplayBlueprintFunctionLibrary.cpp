// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayBlueprintFunctionLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"


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