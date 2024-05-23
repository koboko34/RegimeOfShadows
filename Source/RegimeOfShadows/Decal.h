// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Decal.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API ADecal : public AActor
{
	GENERATED_BODY()
	
public:	
	ADecal();

	UPROPERTY(EditAnywhere)
	class UDecalComponent* Decal;

};
