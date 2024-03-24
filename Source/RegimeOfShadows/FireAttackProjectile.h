// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "FireAttackProjectile.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API AFireAttackProjectile : public AProjectile
{
	GENERATED_BODY()

	AFireAttackProjectile();

private:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UParticleSystem* ExplosionParticleSystem;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float ExplosionVolume = 0.5f;
};
