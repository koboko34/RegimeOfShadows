// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Blueprint/UserWidget.h"

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UUserWidget* HUD = CreateWidget(this, HUDClass, TEXT("HUD"));
	HUD->AddToViewport();
}
