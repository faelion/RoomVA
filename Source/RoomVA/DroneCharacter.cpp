#include "DroneCharacter.h"

#include "Camera/CameraComponent.h"
#include "Camera/PlayerCameraManager.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "UObject/ConstructorHelpers.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "Engine/SkeletalMesh.h"
#include "Animation/AnimationAsset.h"

#include "Trash.h"
#include "RoomVAGameMode.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/EngineTypes.h"

ADroneCharacter::ADroneCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// Camera fully drives pawn rotation.
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = true;
	bUseControllerRotationRoll = false;

	// Capsule: keep small, no step-up needed while flying.
	GetCapsuleComponent()->InitCapsuleSize(21.f, 21.f);

	// Spring arm.
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(GetCapsuleComponent());
	CameraBoom->TargetArmLength = 70.f;
	CameraBoom->bUsePawnControlRotation = true;
	CameraBoom->ProbeSize = 4.f;          // hug walls in narrow spaces
	CameraBoom->bEnableCameraLag = false; // off: avoids floating through walls in tight gaps

	// Camera.
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	// Flying movement + hover easing.
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	MoveComp->DefaultLandMovementMode = MOVE_Flying;
	MoveComp->GravityScale = 0.f;
	MoveComp->MaxFlySpeed = 300.f;               // halved: level is small/tight
	MoveComp->BrakingDecelerationFlying = 256.f; // coast to stop = hover
	MoveComp->MaxAcceleration = 750.f;           // halved to match slower top speed
	MoveComp->bOrientRotationToMovement = false;

	// ---- Auto-load assets by path (zero editor wiring if paths match) ----
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MeshObj(
		TEXT("/Game/ThirdPerson/Assets/Player/mech_drone/SkeletalMeshes/mech_drone.mech_drone"));
	if (MeshObj.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(MeshObj.Object);
		// Default offset; tweak in BP child if it sits wrong.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.f, 0.f, -42.f), FRotator(0.f, -90.f, 0.f));
	}

	static ConstructorHelpers::FObjectFinder<UAnimationAsset> AnimObj(
		TEXT("/Game/ThirdPerson/Assets/Player/mech_drone/SkeletalMeshes/mech_drone_Anim.mech_drone_Anim"));
	if (AnimObj.Succeeded())
	{
		IdleAnim = AnimObj.Object;
		GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);
	}

	static ConstructorHelpers::FObjectFinder<UInputMappingContext> ImcObj(
		TEXT("/Game/Input/IMC_Default.IMC_Default"));
	if (ImcObj.Succeeded()) { DefaultMappingContext = ImcObj.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> MoveObj(
		TEXT("/Game/Input/Actions/IA_Move.IA_Move"));
	if (MoveObj.Succeeded()) { MoveAction = MoveObj.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> LookObj(
		TEXT("/Game/Input/Actions/IA_Look.IA_Look"));
	if (LookObj.Succeeded()) { LookAction = LookObj.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> MouseLookObj(
		TEXT("/Game/Input/Actions/IA_MouseLook.IA_MouseLook"));
	if (MouseLookObj.Succeeded()) { MouseLookAction = MouseLookObj.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> VertObj(
		TEXT("/Game/Input/Actions/IA_Fly_Vertical.IA_Fly_Vertical"));
	if (VertObj.Succeeded()) { VerticalAction = VertObj.Object; }

	static ConstructorHelpers::FObjectFinder<UInputAction> AbsorbObj(
		TEXT("/Game/Input/Actions/IA_Absorb.IA_Absorb"));
	if (AbsorbObj.Succeeded()) { AbsorbAction = AbsorbObj.Object; }
}

void ADroneCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (IdleAnim)
	{
		GetMesh()->PlayAnimation(IdleAnim, /*bLooping=*/true);
	}

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsys =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsys->AddMappingContext(DefaultMappingContext, 0);
			}
		}
		if (PC->PlayerCameraManager)
		{
			PC->PlayerCameraManager->ViewPitchMin = CameraPitchMin;
			PC->PlayerCameraManager->ViewPitchMax = CameraPitchMax;
		}
	}
}

void ADroneCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Extra damping on vertical velocity only while NOT actively pushing up/down.
	// Engine flying brake already removes BrakingDecelerationFlying per second on all axes;
	// here we add (mult-1)x more on Z so up/down settles faster without affecting horizontal feel.
	if (!bVerticalInputThisFrame && VerticalDampingMultiplier > 1.f)
	{
		UCharacterMovementComponent* MoveComp = GetCharacterMovement();
		FVector Vel = MoveComp->Velocity;
		const float ExtraDecel = MoveComp->BrakingDecelerationFlying * (VerticalDampingMultiplier - 1.f);
		const float dv = ExtraDecel * DeltaSeconds;
		if (FMath::Abs(Vel.Z) <= dv)
		{
			Vel.Z = 0.f;
		}
		else
		{
			Vel.Z -= FMath::Sign(Vel.Z) * dv;
		}
		MoveComp->Velocity = Vel;
	}

	bVerticalInputThisFrame = false; // reset each frame

	if (bAbsorbing)
	{
		UpdateAbsorb(DeltaSeconds);
	}
}

void ADroneCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MoveAction)     { EIC->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ADroneCharacter::Move); }
		if (LookAction)     { EIC->BindAction(LookAction, ETriggerEvent::Triggered, this, &ADroneCharacter::Look); }
		if (MouseLookAction){ EIC->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &ADroneCharacter::Look); }
		if (VerticalAction) { EIC->BindAction(VerticalAction, ETriggerEvent::Triggered, this, &ADroneCharacter::Vertical); }
		if (AbsorbAction)
		{
			EIC->BindAction(AbsorbAction, ETriggerEvent::Started, this, &ADroneCharacter::StartAbsorb);
			EIC->BindAction(AbsorbAction, ETriggerEvent::Completed, this, &ADroneCharacter::StopAbsorb);
			EIC->BindAction(AbsorbAction, ETriggerEvent::Canceled, this, &ADroneCharacter::StopAbsorb);
		}
	}
}

void ADroneCharacter::StartAbsorb()
{
	bAbsorbing = true;
}

void ADroneCharacter::StopAbsorb()
{
	bAbsorbing = false;
}

void ADroneCharacter::UpdateAbsorb(float DeltaSeconds)
{
	const FVector Origin = GetActorLocation();
	const FVector CamForward = FollowCamera ? FollowCamera->GetForwardVector()
		: (Controller ? Controller->GetControlRotation().Vector() : GetActorForwardVector());

	// Find trash within range.
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));

	TArray<AActor*> Ignore;
	Ignore.Add(this);

	TArray<AActor*> Found;
	UKismetSystemLibrary::SphereOverlapActors(
		this, Origin, AbsorbRange, ObjectTypes, ATrash::StaticClass(), Ignore, Found);

	const float CosHalfAngle = FMath::Cos(FMath::DegreesToRadians(AbsorbConeHalfAngleDeg));

	for (AActor* Actor : Found)
	{
		ATrash* Trash = Cast<ATrash>(Actor);
		if (!Trash) { continue; }

		const FVector ToTrash = Trash->GetActorLocation() - Origin;
		const float Dist = ToTrash.Size();

		// Within suction cone?
		if (Dist > KINDA_SMALL_NUMBER)
		{
			const float Dot = FVector::DotProduct(ToTrash / Dist, CamForward);
			if (Dot < CosHalfAngle) { continue; } // outside cone
		}

		Trash->bBeingAbsorbed = true;

		if (Dist <= CollectDistance)
		{
			// Collect.
			if (ARoomVAGameMode* GM = Cast<ARoomVAGameMode>(UGameplayStatics::GetGameMode(this)))
			{
				GM->AddCleaned(Trash->TrashValue);
			}
			Trash->Destroy();
		}
		else if (Dist > KINDA_SMALL_NUMBER)
		{
			// Pull via physics velocity so releasing LMB drops it back under gravity.
			Trash->PullToward(Origin, PullSpeed);
		}
	}
}

void ADroneCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) { return; }

	const FVector2D Axis = Value.Get<FVector2D>();

	// Full control rotation (incl pitch) => fly where you look. Roll stays 0.
	const FRotator Rot = Controller->GetControlRotation();
	const FVector Forward = FRotationMatrix(Rot).GetUnitAxis(EAxis::X);
	const FVector Right   = FRotationMatrix(Rot).GetUnitAxis(EAxis::Y);

	AddMovementInput(Forward, Axis.Y); // W/S
	AddMovementInput(Right, Axis.X);   // A/D (stays horizontal: pitch leaves Y axis level)
}

void ADroneCharacter::Look(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	AddControllerYawInput(Axis.X);
	AddControllerPitchInput(Axis.Y);
}

void ADroneCharacter::Vertical(const FInputActionValue& Value)
{
	// World up/down, independent of aim (GDD: Space elevate / Ctrl descend).
	const float V = Value.Get<float>();
	AddMovementInput(FVector::UpVector, V);

	if (!FMath::IsNearlyZero(V))
	{
		bVerticalInputThisFrame = true; // suppress extra Z damping while actively rising/descending
	}
}
