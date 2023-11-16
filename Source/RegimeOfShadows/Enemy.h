// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "Enemy.generated.h"

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

	UPROPERTY(EditAnywhere)
	USceneComponent* Root;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;

public:
	StatusEffects StatusEffects;

	void ApplyBurning();
	void ApplyWet();
	void ApplyFrost();
	void ApplyCharged();
	void ClearStatusEffects();

	UFUNCTION(BlueprintImplementableEvent)
	void ApplyBurningMaterial();
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyWetMaterial();
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyFrostMaterial();
	UFUNCTION(BlueprintImplementableEvent)
	void ApplyChargedMaterial();
	UFUNCTION(BlueprintImplementableEvent)
	void RemoveStatusMaterial();

private:
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ExpOnKill;

	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int StatusDuration = 5;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int FireDamageOnDOT = 1;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ElectricDamageOnDOT = 2;
	int DOTRemainingDuration;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int FireDOTInterval = 1;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ElectricDOTInterval = 2;

	UPROPERTY(EditAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	float DeathDestroyDelay = 10.f;

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

	FTimerHandle DestroyHandle;
	FTimerDelegate DestroyDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* BurningMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* WetMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* FrostMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* ChargedMaterial;

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

	void StartDOT();
	UFUNCTION()
	void ApplyDOT();

	void Death();
	UFUNCTION()
	void DestroyAfterDeath();
	
};
