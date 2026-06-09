#include "NPC.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"

ANPC::ANPC()
{
	PrimaryActorTick.bCanEverTick = false;

	InteractZone = CreateDefaultSubobject<USphereComponent>(TEXT("InteractZone"));
	SetRootComponent(InteractZone);
	InteractZone->InitSphereRadius(250.f);
	InteractZone->SetCollisionEnabled(ECollisionEnabled::NoCollision); // proximity is checked by distance, not collision

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(InteractZone);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	Tags.Add(FName("NPC"));
}
