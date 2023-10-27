// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "InputActionValue.h"
#include "PlayerCharacter.generated.h"

class AProjectile;
class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class AEnemy;
class ASnowGlobe;

UENUM(BlueprintType)
enum class Element
{
	Fire		UMETA(DisplayName = "Fire"),
	Ice			UMETA(DisplayName = "Ice"),
	Electric	UMETA(DisplayName = "Electric")
};

UCLASS()
class REGIMEOFSHADOWS_API APlayerCharacter : public ABaseEntity
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleMapAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OverchargeAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BasicAttackAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BasicAbilityAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StrongAbilityAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapToFireAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapToIceAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapToElectricAction;

private:
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int Experience;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ExpToNextLevel;
	
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	int WalkSpeed = 600;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	int SprintSpeed = 1000;

	Element ActiveElement;

	UPROPERTY(EditDefaultsOnly, Category = FireAbilities, meta = (AllowPrivateAccess))
	TSubclassOf<AProjectile> BasicAttackFireProj;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities, meta = (AllowPrivateAccess))
	int FireBasicManaCost = 15;
	UPROPERTY(EditDefaultsOnly, Category = FireAbilities, meta = (AllowPrivateAccess))
	int FireStrongManaCost = 20;

	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities, meta = (AllowPrivateAccess))
	TSubclassOf<AProjectile> BasicAttackIceProj;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities, meta = (AllowPrivateAccess))
	TSubclassOf<AProjectile> BasicAbilityWaterProj;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities, meta = (AllowPrivateAccess))
	int WaterAbilityManaCost = 2;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities, meta = (AllowPrivateAccess))
	TSubclassOf<ASnowGlobe> SnowGlobeClass;
	UPROPERTY(EditDefaultsOnly, Category = WaterAbilities, meta = (AllowPrivateAccess))
	int SnowGlobeManaCost = 5;

	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities, meta = (AllowPrivateAccess))
	TSubclassOf<AActor> EnemyClass;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities, meta = (AllowPrivateAccess))
	int ElectricChainDamage = 10;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities, meta = (AllowPrivateAccess))
	float ElectricChainSpreadRadius = 300;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities, meta = (AllowPrivateAccess))
	float ElectricChainOverchargeMultiplier = 3.f;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities, meta = (AllowPrivateAccess))
	int ElectricBasicManaCost = 5;
	UPROPERTY(EditDefaultsOnly, Category = ElectricAbilities, meta = (AllowPrivateAccess))
	int ElectricStrongManaCost = 15;

public:
	UCameraComponent* GetCameraComponent() const { return Camera; }

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	void AddExp(int ExpToAdd);
	void AddMana(int ManaToAdd);

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void ToggleMap(const FInputActionValue& Value);

	void BasicAttack(const FInputActionValue& Value);
	void BasicAbility(const FInputActionValue& Value);
	void StrongAbility(const FInputActionValue& Value);

	void BasicAttackFire();
	void BasicAbilityFire();
	void StrongAbilityFire();

	void BasicAttackIce();
	void BasicAbilityIce();
	void StrongAbilityIce();

	void BasicAttackElectric();
	void BasicAbilityElectric();
	void StrongAbilityElectric();

	void SwapToFire(const FInputActionValue& Value);
	void SwapToIce(const FInputActionValue& Value);
	void SwapToElectric(const FInputActionValue& Value);

	void Overcharge(const FInputActionValue& Value);

	void UseMana(int ManaToUse);

	void LevelUp();

	// Ability Declarations

	void ElectricChainStart(AActor* HitActor);
	void ElectricChainRecursive(AActor* HitActor, TArray<AActor*>& HitActors);

	void SpawnProjectile(TSubclassOf<AProjectile> ProjectileToSpawn);

};
