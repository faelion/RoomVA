#include "RoomVAGameMode.h"
#include "Engine/Engine.h"

int32 ARoomVAGameMode::AddCleaned(int32 Amount)
{
	TotalCleaned += Amount;

	// Temporary on-screen feedback until the HUD exists.
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			-1, 1.5f, FColor::Green,
			FString::Printf(TEXT("Cleaned: %d"), TotalCleaned));
	}

	return TotalCleaned;
}
