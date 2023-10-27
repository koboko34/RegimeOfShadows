// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Enemy.generated.h"

/**
 * 
 */

struct StatusEffects {
	bool Burning = false;
	bool Wet = false;
	bool Frost = false;
	bool Charged = false;
};

UCLASS()
class REGIMEOFSHADOWS_API AEnemy : public ABaseEntity
{
	GENERATED_BODY()

public:
	AEnemy();

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

public:
	StatusEffects StatusEffects;

	void ApplyBurning();
	void ApplyWet();
	void ApplyFrost();
	void ApplyCharged();
	void ClearStatusEffects();

	void StartDOT();

private:
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ExpOnKill;

	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int StatusDuration = 5;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int DamageOnDOT = 1;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int DOTDuration = 5;
	int DOTRemainingDuration;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int DOTInterval = 1;

	FTimerHandle ClearBurningHandle;
	FTimerHandle ClearWetHandle;
	FTimerHandle ClearFrostHandle;
	FTimerHandle ClearChargedHandle;

	FTimerDelegate ClearBurningDelegate;
	FTimerDelegate ClearWetDelegate;
	FTimerDelegate ClearFrostDelegate;
	FTimerDelegate ClearChargedDelegate;

	FTimerHandle ApplyDOTHandle;
	FTimerDelegate ApplyDOTDelegate;

protected:
	void GiveKillExp();

private:
	void CalculateKillExp();

	UFUNCTION()
	void ClearBurning();
	UFUNCTION()
	void ClearWet();
	UFUNCTION()
	void ClearFrost();
	UFUNCTION()
	void ClearCharged();

	UFUNCTION()
	void ApplyDOT();
	
};
