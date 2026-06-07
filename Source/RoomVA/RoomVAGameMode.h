#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RoomVAGameMode.generated.h"

/**
 * Central progression state for Vateres Vacuos.
 * Holds the running cleaned count used by the NPC stage gates.
 */
UCLASS()
class ROOMVA_API ARoomVAGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	// Total trash value cleaned so far this session.
	UPROPERTY(BlueprintReadOnly, Category = "Progression")
	int32 TotalCleaned = 0;

	// Add to the cleaned total. Returns the new total.
	UFUNCTION(BlueprintCallable, Category = "Progression")
	int32 AddCleaned(int32 Amount);
};
