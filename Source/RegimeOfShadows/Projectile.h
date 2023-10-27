// Fill out your copyright notice in the Description page of Project Settings.

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
	// Sets default values for this actor's properties
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
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	// void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

};
