#include "RoomVAHUD.h"

#include "RoomVAGameMode.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"
#include "Kismet/GameplayStatics.h"

void ARoomVAHUD::DrawHUD()
{
	Super::DrawHUD();

	const ARoomVAGameMode* GM = Cast<ARoomVAGameMode>(UGameplayStatics::GetGameMode(this));
	if (!GM || !Canvas) { return; }

	const FString Line = FString::Printf(
		TEXT("Collected: %d   Remaining total: %d   Remaining next stage: %d"),
		GM->TotalCleaned,
		GM->GetRemainingTotal(),
		GM->GetRemainingForStage(GM->CurrentStage));

	UFont* Font = GEngine ? GEngine->GetLargeFont() : nullptr;
	if (!Font) { return; }

	float TextW = 0.f, TextH = 0.f;
	GetTextSize(Line, TextW, TextH, Font, TextScale);

	const float X = (Canvas->SizeX - TextW) * 0.5f; // top-center
	const float Y = 24.f;

	// Drop shadow + text for readability on any background.
	DrawText(Line, FLinearColor::Black, X + 2.f, Y + 2.f, Font, TextScale);
	DrawText(Line, FLinearColor::White, X, Y, Font, TextScale);
}
