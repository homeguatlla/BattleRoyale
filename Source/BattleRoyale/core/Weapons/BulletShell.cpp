
#include "BulletShell.h"

#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

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

	Mesh->AddImpulse(GetActorRightVector() * ShellEjectionImpulse);
	Mesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
}

void ABulletShell::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	BP_OnHit();
	if(ShellSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
	}
	Mesh->OnComponentHit.RemoveAll(this);
	if(LifeTime <= 0.0f)
	{
		Destroy();
	}
	else if(!mLifeTimerHandle.IsValid())
	{
		GetWorld()->GetTimerManager().SetTimer(mLifeTimerHandle, this, &ThisClass::OnLifeOver, LifeTime, false);
	}
}

void ABulletShell::OnLifeOver()
{
	GetWorld()->GetTimerManager().ClearTimer(mLifeTimerHandle);
	Destroy();
}

