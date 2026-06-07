#include "Trash.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ATrash::ATrash()
{
	PrimaryActorTick.bCanEverTick = false;

	// Sphere root = physics body (always simulatable, unlike arbitrary imported meshes).
	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(20.f);
	Collision->SetMobility(EComponentMobility::Movable);
	Collision->SetSimulatePhysics(true);
	Collision->SetEnableGravity(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic); // absorb sweep queries this type
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	// Visual mesh, no collision (purely cosmetic).
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(FName("Trash"));
}

void ATrash::PullToward(const FVector& Target, float Speed)
{
	if (!Collision) { return; }
	const FVector Dir = (Target - GetActorLocation()).GetSafeNormal();
	Collision->SetPhysicsLinearVelocity(Dir * Speed);
}
