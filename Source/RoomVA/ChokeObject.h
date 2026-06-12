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

	// Key mechanic: if set, colliding with any actor that has this same tag
	// destroys both this object and the hit actor (e.g. a locked door).
	// Leave as None for ordinary choke objects.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Choke")
	FName KeyTag = NAME_None;

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

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
