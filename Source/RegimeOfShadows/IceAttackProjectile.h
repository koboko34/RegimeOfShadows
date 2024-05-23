// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "IceAttackProjectile.generated.h"

class APlayerCharacter;

UCLASS()
class REGIMEOFSHADOWS_API AIceAttackProjectile : public AProjectile
{
	GENERATED_BODY()

	AIceAttackProjectile();
	
protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int ManaPerTarget = 3;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	USoundBase* IceSound;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float IceSoundVolume = 1.f;

	APlayerCharacter* Player;
};
