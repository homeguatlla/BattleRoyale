// Fill out your copyright notice in the Description page of Project Settings.


#include "AreaProjectile.h"
#include "BattleRoyale/core/Utils/UtilsLibrary.h"
#include "Components/SphereComponent.h"

void AAreaProjectile::DoApplyDamageFrom(AActor* hitActor, const FVector& center) const
{
	//The projectile is still alive at this point (Destroyed() runs right after this call), so its own
	//CollisionComp sits exactly at the explosion origin and blocks the radial damage trace towards everyone.
	if(const auto collisionComp = GetCollisionComp())
	{
		collisionComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	TArray<TEnumAsByte<EObjectTypeQuery>> traceObjectTypes;
	traceObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Pawn));

	utils::UtilsLibrary::ApplyRadialDamage(
		GetWorld(),
		GetInstigator(),
		hitActor,
		center,
		ExplosionRadius,
		{DamageEffect},
		traceObjectTypes,
		UEngineTypes::ConvertToTraceType(ECC_Visibility));
	/*
	TArray<AActor*> charactersFound;
	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		center,
		ExplosionRadius,
		traceObjectTypes,
		ACharacterBase::StaticClass(),
		{},
		charactersFound);
*/
	if(IsDebugEnabled)
	{
		DrawDebugSphere(GetWorld(), center, ExplosionRadius, 20, FColor::Green, false, 3);
	}
	/*
	for(auto&& character : charactersFound)
	{
		if(const auto hurtCharacter = Cast<IICharacter>(character))
		{
			ApplyDamageToCharacter(hurtCharacter);
		}
	}*/
}
