// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "FireBasicProjectile.generated.h"

/**
 * 
 */

class APlayerCharacter;

UCLASS()
class REGIMEOFSHADOWS_API AFireBasicProjectile : public AProjectile
{
	GENERATED_BODY()
	
	AFireBasicProjectile();

public:
	UPROPERTY(EditDefaultsOnly)
	float PassthroughDamage = 10;
	bool bActive = false;

private:
	TArray<AActor*> HitActors;

	APlayerCharacter* PlayerCharacter;

protected:
	virtual void BeginPlay() override;

public:
	void Release();
	void Explode();

private:
	UFUNCTION()
	void OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit);

};
