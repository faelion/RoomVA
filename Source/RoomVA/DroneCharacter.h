#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "DroneCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAnimationAsset;
struct FInputActionValue;

/**
 * Flying cleaning-drone pawn for Vateres Vacuos.
 * 6DOF: WASD camera-relative, Space/Ctrl world up/down, mouse look (pitch-clamped).
 * Movement = MOVE_Flying, no gravity, eased accel/decel for hover feel.
 */
UCLASS()
class ROOMVA_API ADroneCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ADroneCharacter();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	// ---- Components ----
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	USpringArmComponent* CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* FollowCamera;

	// ---- Input (auto-loaded by path; override in BP defaults if you move assets) ----
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputMappingContext* DefaultMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* MouseLookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* VerticalAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* AbsorbAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	UInputAction* InteractAction;

	// ---- Tuning ----
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float CameraPitchMin = -85.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float CameraPitchMax = 85.f;

	// Extra braking applied to vertical (Z) velocity when no Space/Ctrl input is held.
	// 1.0 = engine default. 2.0 = double damping (settles ~2x faster on up/down).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Drone")
	float VerticalDampingMultiplier = 2.f;

	// ---- Absorb (vacuum) ----
	// Max distance the absorb reaches.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Absorb")
	float AbsorbRange = 300.f;

	// Half-angle of the suction cone around camera-forward (degrees). Trash outside is ignored.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Absorb")
	float AbsorbConeHalfAngleDeg = 25.f;

	// Speed (cm/s) trash flies toward the drone while being absorbed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Absorb")
	float PullSpeed = 300.f;

	// Distance at which trash is collected and removed.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Absorb")
	float CollectDistance = 50.f;

	// How close the drone must be to an NPC to interact with it.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interact")
	float InteractReach = 300.f;

private:
	// True on frames where the vertical (Space/Ctrl) action fired. Drives the extra Z damping.
	bool bVerticalInputThisFrame = false;

	// True while the absorb input is held.
	bool bAbsorbing = false;

	void UpdateAbsorb(float DeltaSeconds);

protected:

	// Idle anim played on the drone mesh (single-node mode).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Drone")
	UAnimationAsset* IdleAnim;

	// ---- Input handlers ----
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Vertical(const FInputActionValue& Value);
	void StartAbsorb();
	void StopAbsorb();
	void Interact();
};
