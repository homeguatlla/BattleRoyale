
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BulletShell.generated.h"

UCLASS()
class BATTLEROYALE_API ABulletShell : public AActor
{
	GENERATED_BODY()
	
public:
	ABulletShell();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "BulletShell", meta = (DisplayName = OnHit))
	void BP_OnHit();

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse = 100.0f;

protected:
	virtual void BeginPlay() override;
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
private:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

};
