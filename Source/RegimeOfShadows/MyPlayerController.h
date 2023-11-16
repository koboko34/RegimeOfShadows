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

protected:
	virtual void BeginPlay() override;

private:
};
