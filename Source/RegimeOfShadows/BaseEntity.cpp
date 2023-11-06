// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEntity.h"

// Sets default values
ABaseEntity::ABaseEntity()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Health = MaxHealth;
	Mana = MaxMana;
	Stamina = MaxStamina;
}

// Called when the game starts or when spawned
void ABaseEntity::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ABaseEntity::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseEntity::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (DamageCauser == this)
	{
		if (DamageAmount > Health)
		{
			Health = 0;
		}
		else
		{
			Health -= DamageAmount;
		}
		return DamageAmount;
	}
	
	ABaseEntity* DamagingEntity = Cast<ABaseEntity>(DamageCauser);
	int AdjustedDamage = DamageAmount;
	if (DamagingEntity)
	{
		AdjustedDamage *= (1 + DamagingEntity->CalcRageFactor());
	}
	
	if (AdjustedDamage > Health)
	{
		Health = 0;
	}
	else
	{
		Health -= AdjustedDamage;
	}
	return AdjustedDamage;
}

float ABaseEntity::CalcRageFactor() const
{
	if (!bIsAlive)
		return 0;

	return 1 - ((float)Health / (float)MaxHealth);
}
