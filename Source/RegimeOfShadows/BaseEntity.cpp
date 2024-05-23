// Armand Yilinkou, 2023


#include "BaseEntity.h"
#include "LifeStealInterface.h"

ABaseEntity::ABaseEntity()
{
	PrimaryActorTick.bCanEverTick = true;

	Health = MaxHealth;
	Mana = MaxMana;
	Stamina = MaxStamina;
}

void ABaseEntity::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

float ABaseEntity::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	ABaseEntity* DamagingEntity = Cast<ABaseEntity>(DamageCauser);
	int AdjustedDamage = DamageAmount;
	if (DamagingEntity)
	{
		AdjustedDamage *= (1 + DamagingEntity->CalcRageFactor());
	}
	
	if (AdjustedDamage > Health)
	{
		Health = 0;
		bIsAlive = false;
	}
	else
	{
		Health -= AdjustedDamage;
	}

	ILifeStealInterface* LifeStealInterface = Cast<ILifeStealInterface>(DamageCauser);
	if (LifeStealInterface)
		LifeStealInterface->ApplyLifeSteal(AdjustedDamage);

	return AdjustedDamage;
}

float ABaseEntity::CalcRageFactor() const
{
	if (!bIsAlive)
		return 0;

	return 1 - ((float)Health / (float)MaxHealth);
}

void ABaseEntity::SetHealth(int HealthToSet)
{
	Health = HealthToSet;
}
