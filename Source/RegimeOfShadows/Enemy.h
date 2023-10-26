// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Enemy.generated.h"

/**
 * 
 */
UCLASS()
class REGIMEOFSHADOWS_API AEnemy : public ABaseEntity
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

private:
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ExpOnKill;

protected:
	void GiveKillExp();

private:
	void CalculateKillExp();
	
};
