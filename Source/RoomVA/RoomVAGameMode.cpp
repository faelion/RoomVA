#include "RoomVAGameMode.h"

#include "Trash.h"
#include "RoomVAHUD.h"
#include "Engine/Engine.h"
#include "Kismet/GameplayStatics.h"

ARoomVAGameMode::ARoomVAGameMode()
{
	HUDClass = ARoomVAHUD::StaticClass();
}

void ARoomVAGameMode::BeginPlay()
{
	Super::BeginPlay();

	// Tally trash per stage.
	TArray<AActor*> AllTrash;
	UGameplayStatics::GetAllActorsOfClass(this, ATrash::StaticClass(), AllTrash);
	for (AActor* A : AllTrash)
	{
		if (const ATrash* T = Cast<ATrash>(A))
		{
			StageRemaining.FindOrAdd(T->StageIndex) += 1;
		}
	}

	CurrentStage = 1;
	OpenDoorsForStage(1); // room 1 entry open from the start

	Screen(FString::Printf(TEXT("Stage 1 active. Trash to clean: %d"),
		StageRemaining.Contains(1) ? StageRemaining[1] : 0), FColor::Cyan);
}

void ARoomVAGameMode::NotifyTrashCollected(int32 StageIndex, int32 Value)
{
	TotalCleaned += Value;

	if (int32* Remaining = StageRemaining.Find(StageIndex))
	{
		*Remaining = FMath::Max(0, *Remaining - 1);
	}
	// (Per-collect feedback now lives on the HUD.)
}

int32 ARoomVAGameMode::GetRemainingTotal() const
{
	int32 Total = 0;
	for (const TPair<int32, int32>& Pair : StageRemaining)
	{
		Total += Pair.Value;
	}
	return Total;
}

int32 ARoomVAGameMode::GetRemainingForStage(int32 StageIndex) const
{
	const int32* Remaining = StageRemaining.Find(StageIndex);
	return Remaining ? *Remaining : 0;
}

bool ARoomVAGameMode::IsStageCleared(int32 StageIndex) const
{
	const int32* Remaining = StageRemaining.Find(StageIndex);
	return (Remaining == nullptr) || (*Remaining <= 0);
}

bool ARoomVAGameMode::TryAdvanceStage()
{
	if (!IsStageCleared(CurrentStage))
	{
		const int32 Left = StageRemaining.Contains(CurrentStage) ? StageRemaining[CurrentStage] : 0;
		Screen(FString::Printf(TEXT("Clean stage %d first (%d left)"), CurrentStage, Left), FColor::Orange);
		return false;
	}

	++CurrentStage;
	OpenDoorsForStage(CurrentStage);
	Screen(FString::Printf(TEXT("Stage %d doors opened!"), CurrentStage), FColor::Yellow);
	return true;
}

void ARoomVAGameMode::OpenDoorsForStage(int32 StageIndex)
{
	const FName Tag(*FString::Printf(TEXT("OpenStage%d"), StageIndex));

	TArray<AActor*> Doors;
	UGameplayStatics::GetAllActorsWithTag(this, Tag, Doors);
	for (AActor* Door : Doors)
	{
		if (Door)
		{
			Door->Destroy();
		}
	}
}

void ARoomVAGameMode::Screen(const FString& Msg, const FColor& Color) const
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.5f, Color, Msg);
	}
}
