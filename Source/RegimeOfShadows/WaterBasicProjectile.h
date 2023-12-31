// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "WaterBasicProjectile.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API AWaterBasicProjectile : public AProjectile
{
	GENERATED_BODY()
	
	AWaterBasicProjectile();

private:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);
};
