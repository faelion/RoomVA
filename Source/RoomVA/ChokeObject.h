#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChokeObject.generated.h"

class USphereComponent;
class UStaticMeshComponent;

/**
 * An object that can be vacuumed but is too big to swallow: it gets stuck in the
 * drone's mouth. Released (LMB up) it falls; spat (RMB) it is launched forward.
 */
UCLASS()
class ROOMVA_API AChokeObject : public AActor
{
	GENERATED_BODY()

public:
	AChokeObject();

	// True while plugged into the drone's mouth.
	UPROPERTY(BlueprintReadOnly, Category = "Choke")
	bool bHeld = false;

	// Physics body + what the absorb sweep detects.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Choke")
	USphereComponent* Collision;

	// Visual only; assign mesh on BP children.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Choke")
	UStaticMeshComponent* Mesh;

	// Fling toward Target at Speed (cm/s) while being absorbed.
	void PullToward(const FVector& Target, float Speed);

	// Attach to the drone's mouth (physics off).
	void Hold(USceneComponent* AttachTo, const FName& Socket, const FVector& RelativeOffset);

	// Detach and re-enable physics. Optional launch velocity (spit).
	void Release(const FVector& LaunchVelocity = FVector::ZeroVector);
};
