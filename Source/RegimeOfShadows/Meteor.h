// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Meteor.generated.h"

class USphereComponent;

UCLASS()
class REGIMEOFSHADOWS_API AMeteor : public AProjectile
{
	GENERATED_BODY()
	
public:	
	AMeteor();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

private:
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	FVector ProjVelocity = FVector(1.f, 2.f, -3.f);
	UPROPERTY(EditAnywhere, meta = (AllowPrivateAccess = true))
	float ProjSpeed = 600.f;

	// MUST BE SAME VALUE AS IN ABILITY COMPONENT!
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float HeightOffset = 1000.f;

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	UParticleSystem* ExplosionParticleSystem;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	USoundBase* ExplosionSound;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float ExplosionVolume = 0.5f;
};
