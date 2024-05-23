// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"

class APlayerCharacter;
class AProjectile;
class ASnowGlobe;
class AFireBasicProjectile;
class AElectricPortal;
class ADecal;
class UNiagaraSystem;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REGIMEOFSHADOWS_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UAbilityComponent();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	TSubclassOf<AProjectile> FireAttackProj;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	float FireAttackCooldown = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	TSubclassOf<AProjectile> FireQProj;
	AFireBasicProjectile* ActiveFireQProj = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	TSubclassOf<AProjectile> MeteorClass;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	TSubclassOf<ADecal> MeteorDecalClass;
	ADecal* MeteorDecal = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	float DecalDepth = 512.f;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int MeteorShowerDuration = 10;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	float MeteorSpawnInterval = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	float MeteorSpawnRadius = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	float MeteorShowerHeightOffset = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	float MeteorShowerCastDuration = 2.f;
	FTimerHandle MeteorShowerHandle;
	FTimerDelegate MeteorShowerDelegate;
	float MeteorShowerTimeRemaining;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int FireQManaCost = 15;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int FireQCooldown = 6;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int FireEManaCost = 20;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int FireECooldown = 6;

	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	TSubclassOf<AProjectile> IceAttackProj;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	float IceAttackCooldown = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	TSubclassOf<AProjectile> WaterAbilityProj;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	int WaterAbilityManaCost = 2;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	int WaterAbilityCooldown = 5;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	TSubclassOf<ASnowGlobe> SnowGlobeClass;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	int SnowGlobeManaCost = 5;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	int SnowGlobeCooldown = 1;

	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	TSubclassOf<AActor> EnemyClass;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float OverloadedMultiplier = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricAttackDamage = 5;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float ElectricAttackRadius = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float ElectricAttackHalfHeight = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float ElectricAttackDistance = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float ElectricAttackCooldown = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricChainDamage = 10;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float ElectricChainSpreadRadius = 300;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricBasicManaCost = 5;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricQCooldown = 5;
	FTimerHandle PortalSpawnHandle;
	FTimerDelegate PortalSpawnDelegate;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	TSubclassOf<AElectricPortal> ElectricPortalClass;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	TSubclassOf<AActor> PortalMarkerClass;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float PortalSpawnTimeout = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float PortalSpawnTraceDistance = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float PortalDuration = 5.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float PortalSweepRadius = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int PortalDamage = 5;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricStrongManaCost = 15;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricECooldown = 15;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	UNiagaraSystem* ElectricBeamSystem;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* LaserZapSound;
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	float LaserZapVolume = 1.f;

private:
	APlayerCharacter* PlayerCharacter;

	FVector FirstPortalSpawnLocation;

	AActor* PortalMarker;

	FTimerHandle BasicAttackFireHandle;
	FTimerDelegate BasicAttackFireDelegate;
	FTimerHandle BasicAbilityFireHandle;
	FTimerDelegate BasicAbilityFireDelegate;
	FTimerHandle StrongAbilityFireHandle;
	FTimerDelegate StrongAbilityFireDelegate;
	FTimerHandle BasicAttackIceHandle;
	FTimerDelegate BasicAttackIceDelegate;
	FTimerHandle BasicAbilityIceHandle;
	FTimerDelegate BasicAbilityIceDelegate;
	FTimerHandle StrongAbilityIceHandle;
	FTimerDelegate StrongAbilityIceDelegate;
	FTimerHandle BasicAttackElectricHandle;
	FTimerDelegate BasicAttackElectricDelegate;
	FTimerHandle BasicAbilityElectricHandle;
	FTimerDelegate BasicAbilityElectricDelegate;
	FTimerHandle StrongAbilityElectricHandle;
	FTimerDelegate StrongAbilityElectricDelegate;

	FTimerHandle MeteorShowerCastHandle;
	FTimerDelegate MeteorShowerCastDelegate;

	FTimerHandle SweepWeaponHandle;
	FTimerDelegate SweepWeaponDelegate;

	float* SpellSpeed;
	FVector MeteorLocation;
	bool bCancelAbility = false;

public:
	void BasicAttackFire();
	void BasicAbilityFire();
	void BasicAbilityFireEnd();
	void StrongAbilityFire();
	void StrongAbilityFireEnd();

	void BasicAttackIce();
	void BasicAbilityIce();
	void StrongAbilityIce();

	void BasicAttackElectric();
	void BasicAbilityElectric();
	void StrongAbilityElectric();

	float GetSpellSpeed() const { return *SpellSpeed; }

	void CancelAbilityCast();
	void CancelAbility();

private:
	UFUNCTION()
	void MeteorShowerCast();
	UFUNCTION()
	void MeteorShower();
	
	void ElectricChainStart(AActor* HitActor);
	void ElectricChainRecursive(AActor* HitActor, TArray<AActor*>& HitActors);

	UFUNCTION()
	void ElectricBasicSweep();

	UFUNCTION()
	void CancelPortalSpawn();
	void ClearPortalMarker();

	void TriggerCooldown(FTimerHandle& AbilityHandle, float Duration);

	AProjectile* SpawnProjectile(TSubclassOf<AProjectile> ProjectileToSpawn);

public:
	UFUNCTION(BlueprintPure)
	float GetFireAttackCooldown() const;
	UFUNCTION(BlueprintPure)
	float GetFireQCooldown() const;
	UFUNCTION(BlueprintPure)
	float GetFireECooldown() const;

	UFUNCTION(BlueprintPure)
	float GetIceAttackCooldown() const;
	UFUNCTION(BlueprintPure)
	float GetIceQCooldown() const;
	UFUNCTION(BlueprintPure)
	float GetIceECooldown() const;

	UFUNCTION(BlueprintPure)
	float GetElectricAttackCooldown() const;
	UFUNCTION(BlueprintPure)
	float GetElectricQCooldown() const;
	UFUNCTION(BlueprintPure)
	float GetElectricECooldown() const;

	UFUNCTION(BlueprintPure)
	float GetCastProgress() const;
};
