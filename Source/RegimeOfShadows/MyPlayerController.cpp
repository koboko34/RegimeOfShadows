// Armand Yilinkou, 2023


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	HUDWidget = CreateWidget(this, HUDClass, TEXT("HUD"));
	if (HUDWidget)
	{
		HUDWidget->AddToViewport();
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to create HUD!\n"));
	}
}
