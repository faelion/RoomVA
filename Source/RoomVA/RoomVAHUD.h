#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "RoomVAHUD.generated.h"

/**
 * Minimal canvas HUD: collected / remaining counters pinned to the top of the screen.
 */
UCLASS()
class ROOMVA_API ARoomVAHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	// Text scale multiplier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HUD")
	float TextScale = 1.4f;
};
