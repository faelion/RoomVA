#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NPC.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * Interaction marker. Place one in the level (the "upgrade center" hub).
 * The drone presses Interact (E) while within range to advance the stage.
 */
UCLASS()
class ROOMVA_API ANPC : public AActor
{
	GENERATED_BODY()

public:
	ANPC();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	USphereComponent* InteractZone;

	// Optional visible body (assign the trashbot mesh on a BP child, or leave empty).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "NPC")
	UStaticMeshComponent* Mesh;
};
