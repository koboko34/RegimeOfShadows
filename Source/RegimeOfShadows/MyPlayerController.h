// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
private:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	TSubclassOf<UUserWidget> HUDClass;
	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	UUserWidget* HUDWidget;

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintPure)
	UUserWidget* GetHUDWidget() const { return HUDWidget; }
};
