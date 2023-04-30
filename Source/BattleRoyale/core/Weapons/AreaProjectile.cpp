// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaProjectile.h"

#include "BattleRoyale/core/Character/CharacterBase.h"
#include "Kismet/GameplayStatics.h"

void AAreaProjectile::DoApplyDamageFrom(AActor* hitActor, const FVector& center) const
{
	TArray<AActor*> charactersFound;

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		center,
		ExplosionRadius,
		traceObjectTypes,
		ACharacterBase::StaticClass(),
		{},
		charactersFound);

	if(IsDebugEnabled)
	{
		DrawDebugSphere(GetWorld(), center, ExplosionRadius, 20, FColor::Green, false, 3);
	}
	
	for(auto&& character : charactersFound)
	{
		if(const auto hurtCharacter = Cast<IICharacter>(character))
		{
			ApplyDamageToCharacter(hurtCharacter);
		}
	}
}
