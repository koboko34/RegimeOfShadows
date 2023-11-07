// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseEntity.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API ABaseEntity : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABaseEntity();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	UPROPERTY(VisibleInstanceOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Health = 50;
	UPROPERTY(VisibleInstanceOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Mana = 50;
	UPROPERTY(VisibleInstanceOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	float Stamina = 50;

	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int MaxHealth = 100;
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int MaxMana = 100;
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int MaxStamina = 100;

	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Level = 1;
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Strength = 1;
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Intelligence = 1;
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Vitality = 1;
	UPROPERTY(EditDefaultsOnly, Category = Stats, meta = (AllowPrivateAccess = true))
	int Speed = 1;

	bool bIsAlive = true;

public:
	UFUNCTION(BlueprintPure)
	int GetHealth() const { return Health; }
	UFUNCTION(BlueprintPure)
	int GetMana() const { return Mana; }
	UFUNCTION(BlueprintPure)
	int GetStamina() const { return Stamina; }
	UFUNCTION(BlueprintPure)
	int GetMaxHealth() const { return MaxHealth; }
	UFUNCTION(BlueprintPure)
	int GetMaxMana() const { return MaxMana; }
	UFUNCTION(BlueprintPure)
	int GetMaxStamina() const { return MaxStamina; }
	UFUNCTION(BlueprintPure)
	int GetEntityLevel() const { return Level; }
	UFUNCTION(BlueprintPure)
	int GetStrength() const { return Strength; }
	UFUNCTION(BlueprintPure)
	int GetIntelligence() const { return Intelligence; }
	UFUNCTION(BlueprintPure)
	int GetVitality() const { return Vitality; }
	UFUNCTION(BlueprintPure)
	int GetSpeed() const { return Speed; }

	UFUNCTION(BlueprintPure)
	bool GetIsAlive() const { return bIsAlive; }
	float CalcRageFactor() const;

};
