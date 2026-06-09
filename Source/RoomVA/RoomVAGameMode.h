#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RoomVAGameMode.generated.h"

/**
 * Central progression for Vateres Vacuos.
 * Tracks trash remaining per stage and opens doors (tagged actors) when the
 * player clears a stage and interacts with the NPC.
 *
 * Door convention: actors tagged "OpenStage{N}" are destroyed ("opened") when
 * stage N becomes active. Stage 1 opens at BeginPlay (room 1 entry). Clearing
 * stage 1 + interact opens OpenStage2; clearing stage 2 + interact opens OpenStage3.
 */
UCLASS()
class ROOMVA_API ARoomVAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	// Total trash value cleaned so far.
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 TotalCleaned = 0;

	// Stage the player is currently clearing.
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 CurrentStage = 1;

	// Called by the drone when a trash piece is collected.
	UFUNCTION(BlueprintCallable, Category = "Progression")
	void NotifyTrashCollected(int32 StageIndex, int32 Value);

	// Called by the drone when the player interacts with the NPC.
	// Returns true if a stage was advanced (doors opened).
	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool TryAdvanceStage();

	// True if every trash piece of the given stage has been collected.
	UFUNCTION(BlueprintCallable, Category = "Progression")
	bool IsStageCleared(int32 StageIndex) const;

private:
	// StageIndex -> remaining trash count.
	TMap<int32, int32> StageRemaining;

	void OpenDoorsForStage(int32 StageIndex);
	void Screen(const FString& Msg, const FColor& Color) const;
};
