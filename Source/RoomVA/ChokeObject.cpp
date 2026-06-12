#include "ChokeObject.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

AChokeObject::AChokeObject()
{
	PrimaryActorTick.bCanEverTick = false;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(25.f);
	Collision->SetMobility(EComponentMobility::Movable);
	Collision->SetSimulatePhysics(true);
	Collision->SetEnableGravity(true);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Block);
	Collision->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Collision);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(FName("Choke"));
}

void AChokeObject::BeginPlay()
{
	Super::BeginPlay();

	if (Collision)
	{
		Collision->SetNotifyRigidBodyCollision(true); // generate hit events while simulating
		Collision->OnComponentHit.AddDynamic(this, &AChokeObject::OnHit);
	}
}

void AChokeObject::OnHit(UPrimitiveComponent* /*HitComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, FVector /*NormalImpulse*/, const FHitResult& /*Hit*/)
{
	if (KeyTag.IsNone() || bHeld || !OtherActor) { return; }

	if (OtherActor->ActorHasTag(KeyTag))
	{
		OtherActor->Destroy(); // unlock: remove the door (and any tagged blocker)
		Destroy();             // key is consumed
	}
}

void AChokeObject::PullToward(const FVector& Target, float Speed)
{
	if (!Collision || bHeld) { return; }
	const FVector Dir = (Target - GetActorLocation()).GetSafeNormal();
	Collision->SetPhysicsLinearVelocity(Dir * Speed);
}

void AChokeObject::Hold(USceneComponent* AttachTo, const FName& Socket, const FVector& RelativeOffset)
{
	if (!Collision || !AttachTo) { return; }

	bHeld = true;
	Collision->SetSimulatePhysics(false);
	Collision->SetCollisionEnabled(ECollisionEnabled::NoCollision); // don't push the drone around
	AttachToComponent(AttachTo, FAttachmentTransformRules::SnapToTargetNotIncludingScale, Socket);
	SetActorRelativeLocation(RelativeOffset);
}

void AChokeObject::Release(const FVector& LaunchVelocity)
{
	if (!Collision) { return; }

	bHeld = false;
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Collision->SetSimulatePhysics(true);

	if (!LaunchVelocity.IsNearlyZero())
	{
		Collision->SetPhysicsLinearVelocity(LaunchVelocity);
	}
}
