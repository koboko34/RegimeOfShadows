// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

AEnemy::AEnemy()
{
	CalculateKillExp();
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %i"), Health);
	if (Health <= 0)
	{
		Destroy();
	}
	return DamageAmount;
}

void AEnemy::GiveKillExp()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->AddExp(ExpOnKill);
}

void AEnemy::CalculateKillExp()
{
	ExpOnKill = Level * 10;
}
