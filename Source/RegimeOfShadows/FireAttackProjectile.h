// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "FireAttackProjectile.generated.h"

/**
 * 
 */
UCLASS()
class REGIMEOFSHADOWS_API AFireAttackProjectile : public AProjectile
{
	GENERATED_BODY()

		AFireAttackProjectile();

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageType> DamageType;

private:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

};
