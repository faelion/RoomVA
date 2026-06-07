#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Trash.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * An absorbable trash object. A sphere collision root simulates physics (so it
 * falls, rests, and can be sucked in) regardless of the visual mesh's collision.
 * Assign the visual in the Mesh component on a BP child.
 */
UCLASS()
class ROOMVA_API ATrash : public AActor
{
	GENERATED_BODY()

public:
	ATrash();

	// How much this piece counts toward the cleaned total.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trash")
	int32 TrashValue = 1;

	// True while being sucked in.
	UPROPERTY(BlueprintReadOnly, Category = "Trash")
	bool bBeingAbsorbed = false;

	// Fling this trash toward Target at Speed (cm/s) using physics velocity.
	void PullToward(const FVector& Target, float Speed);

	UStaticMeshComponent* GetMesh() const { return Mesh; }

protected:
	// Physics body + what the absorb sweep detects.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trash")
	USphereComponent* Collision;

	// Visual only (no collision); assign the static mesh here on BP children.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trash")
	UStaticMeshComponent* Mesh;
};
