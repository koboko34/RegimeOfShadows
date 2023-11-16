// Armand Yilinkou, 2023


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UUserWidget* HUD = CreateWidget(this, HUDClass, TEXT("HUD"));
	if (HUD)
	{
		HUD->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create HUD!\n"));
	}
}
