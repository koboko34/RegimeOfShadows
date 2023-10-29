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
	int Stamina = 50;

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
	int GetHealth() const { return Health; }
	int GetMana() const { return Mana; }
	int GetStamina() const { return Stamina; }
	int GetMaxHealth() const { return MaxHealth; }
	int GetMaxMana() const { return MaxMana; }
	int GetMaxStamina() const { return MaxStamina; }
	int GetLevel() const { return Level; }
	int GetStrength() const { return Strength; }
	int GetIntelligence() const { return Intelligence; }
	int GetVitality() const { return Vitality; }
	int GetSpeed() const { return Speed; }

	bool GetIsAlive() const { return bIsAlive; }
	float CalcRageFactor() const;

};
