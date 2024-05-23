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

class UPawnSensingComponent;
class UBlackboardComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHearNoiseDelegate, APawn*, Instigator, const FVector&, Location, float, Volume);

UCLASS()
class REGIMEOFSHADOWS_API AEnemy : public ABaseEntity
{
	GENERATED_BODY()

public:
	AEnemy();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USceneComponent* AttackPoint;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	UPawnSensingComponent* PawnSensingComponent;
	
	UBlackboardComponent* BlackboardComponent;

	UPROPERTY(BlueprintReadOnly)
	FVector InitialLocation;

	UPROPERTY(EditAnywhere)
	float ChaseRange = 2000.f;
	UPROPERTY(EditDefaultsOnly)
	float BaseMovementSpeed = 600.f;
	UPROPERTY(EditDefaultsOnly)
	float FrostSpeedDebuff = 0.5f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bIsBoss = false;

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

	UFUNCTION()
	void OnHearNoise(APawn* OtherActor, const FVector& Location, float Volume);

	UFUNCTION(BlueprintCallable)
	void GiveKillExp();

	UFUNCTION(BlueprintCallable)
	void SetHasScreamed(bool NewHasScreamed);
	UFUNCTION(BlueprintPure)
	bool GetCanScream() const { return bCanScream; }
	UFUNCTION(BlueprintCallable)
	void SetInChase(bool NewInChase);
	UFUNCTION(BlueprintPure)
	bool IsInChase() const { return bInChase; }

	UFUNCTION(BlueprintPure)
	float GetMoveAcceptanceRadius() const { return MoveAcceptanceRadius; }
	UFUNCTION(BlueprintPure)
	float GetHearingRange() const { return HearingRange; }

	UFUNCTION(BlueprintCallable)
	void SetIsAwake(bool NewIsAwake);
	UFUNCTION(BlueprintPure)
	bool GetIsAwake() const { return bIsAwake; }


private:	
	UPROPERTY(EditAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ExpOnKill = 12;
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
	UPROPERTY(EditAnywhere, Category = Boss, meta = (AllowPrivateAccess = true))
	float BossStageHealthThreshold = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = Boss, meta = (AllowPrivateAccess = true))
	bool bCanScream = false;

	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = true))
	float HearingRange = 2000.f;
	UPROPERTY(EditAnywhere, Category = AI, meta = (AllowPrivateAccess = true))
	float MoveAcceptanceRadius = 200.f;

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

	FTimerHandle RangeCheckHandle;
	FTimerDelegate RangeCheckDelegate;

	FTimerHandle AliveSoundHandle;
	FTimerDelegate AliveSoundDelegate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* BurningMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* WetMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* FrostMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effects, meta = (AllowPrivateAccess = true))
	UMaterialInterface* ChargedMaterial;

	bool bHasScreamed = false;
	bool bInChase = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds, meta = (AllowPrivateAccess = true))
	USoundBase* DeathSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds, meta = (AllowPrivateAccess = true))
	USoundBase* AliveSound;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds, meta = (AllowPrivateAccess = true))
	float DeathSoundVolume = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Sounds, meta = (AllowPrivateAccess = true))
	float AliveSoundVolume = 1.f;

	UPROPERTY(EditDefaultsOnly, Category = Boss, meta = (AllowPrivateAccess = true))
	bool bIsAwake = true;

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

	UFUNCTION()
	void PlayAliveSound();

	UFUNCTION()
	void RangeCheck();
	
	bool InChaseRange();
};
