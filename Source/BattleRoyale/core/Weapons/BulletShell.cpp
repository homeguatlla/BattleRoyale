
#include "BulletShell.h"

ABulletShell::ABulletShell()
{
 	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletShellMesh"));
	SetRootComponent(Mesh);

	Mesh->SetSimulatePhysics(true);
	Mesh->SetEnableGravity(true);
	Mesh->SetNotifyRigidBodyCollision(true);
}

void ABulletShell::BeginPlay()
{
	Super::BeginPlay();

	Mesh->AddImpulse(GetActorForwardVector() * ShellEjectionImpulse);
	Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnHit();
}

