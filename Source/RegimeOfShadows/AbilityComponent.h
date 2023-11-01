// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilityComponent.generated.h"

class APlayerCharacter;
class AProjectile;
class ASnowGlobe;
class AFireBasicProjectile;
class AElectricPortal;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REGIMEOFSHADOWS_API UAbilityComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAbilityComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	TSubclassOf<AProjectile> FireAttackProj;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	TSubclassOf<AProjectile> FireQProj;
	AFireBasicProjectile* ActiveFireQProj = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int FireQManaCost = 15;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities)
	int FireEManaCost = 20;

	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	TSubclassOf<AProjectile> IceAttackProj;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	TSubclassOf<AProjectile> WaterAbilityProj;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	int WaterAbilityManaCost = 2;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	TSubclassOf<ASnowGlobe> SnowGlobeClass;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities)
	int SnowGlobeManaCost = 5;

	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	TSubclassOf<AActor> EnemyClass;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float OverloadedMultiplier = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricChainDamage = 10;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	float ElectricChainSpreadRadius = 300;
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
	int ElectricBasicManaCost = 5;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities)
	int ElectricStrongManaCost = 15;

private:
	APlayerCharacter* PlayerCharacter;

	FVector FirstPortalSpawnLocation;

	AActor* PortalMarker;

public:
	void BasicAttackFire();
	void BasicAbilityFire();
	void BasicAbilityFireEnd();
	void StrongAbilityFire();

	void BasicAttackIce();
	void BasicAbilityIce();
	void StrongAbilityIce();

	void BasicAttackElectric();
	void BasicAbilityElectric();
	void StrongAbilityElectric();

private:
	void ElectricChainStart(AActor* HitActor);
	void ElectricChainRecursive(AActor* HitActor, TArray<AActor*>& HitActors);

	UFUNCTION()
	void CancelPortalSpawn();
	void ClearPortalMarker();

	AProjectile* SpawnProjectile(TSubclassOf<AProjectile> ProjectileToSpawn);
};
