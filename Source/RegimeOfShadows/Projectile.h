// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class REGIMEOFSHADOWS_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereCollider;
	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
	UProjectileMovementComponent* ProjectileMovementComponent;

protected:
	UPROPERTY(EditDefaultsOnly)
	int DamageToDeal = 10;
	UPROPERTY(EditDefaultsOnly)
	float ExplosionRadius = 100.f;

protected:
	void HandleExplosion(TArray<FHitResult>& HitResultsOut, bool bDrawDebug, FColor Colour, float DamageMultiplier = 1.f);

};
