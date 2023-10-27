// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

AEnemy::AEnemy()
{
	CalculateKillExp();
	ClearStatusEffects();

	ClearBurningDelegate.BindUObject(this, &AEnemy::ClearBurning);
	ClearWetDelegate.BindUObject(this, &AEnemy::ClearWet);
	ClearFrostDelegate.BindUObject(this, &AEnemy::ClearFrost);
	ClearChargedDelegate.BindUObject(this, &AEnemy::ClearCharged);

	ApplyDOTDelegate.BindUObject(this, &AEnemy::ApplyDOT);
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

void AEnemy::ApplyBurning()
{
	StatusEffects.Burning = true;
	GetWorldTimerManager().SetTimer(ClearBurningHandle, ClearBurningDelegate, StatusDuration, false);
}

void AEnemy::ApplyWet()
{
	StatusEffects.Wet = true;
	GetWorldTimerManager().SetTimer(ClearWetHandle, ClearWetDelegate, StatusDuration, false);
}

void AEnemy::ApplyFrost()
{
	StatusEffects.Frost = true;
	GetWorldTimerManager().SetTimer(ClearFrostHandle, ClearFrostDelegate, StatusDuration, false);
}

void AEnemy::ApplyCharged()
{
	StatusEffects.Charged = true;
	GetWorldTimerManager().SetTimer(ClearChargedHandle, ClearChargedDelegate, StatusDuration, false);
}

void AEnemy::ClearStatusEffects()
{
	StatusEffects.Burning = false;
	StatusEffects.Wet = false;
	StatusEffects.Frost = false;
	StatusEffects.Charged = false;

	if (ClearBurningHandle.IsValid())
		GetWorldTimerManager().ClearTimer(ClearBurningHandle);
	if (ClearWetHandle.IsValid())
		GetWorldTimerManager().ClearTimer(ClearWetHandle);
	if (ClearFrostHandle.IsValid())
		GetWorldTimerManager().ClearTimer(ClearFrostHandle);
	if (ClearChargedHandle.IsValid())
		GetWorldTimerManager().ClearTimer(ClearChargedHandle);
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

void AEnemy::ClearBurning()
{
	StatusEffects.Burning = false;
}

void AEnemy::ClearWet()
{
	UE_LOG(LogTemp, Warning, TEXT("Clearing Wet!"));
	StatusEffects.Wet = false;
}

void AEnemy::ClearFrost()
{
	StatusEffects.Frost = false;
}

void AEnemy::ClearCharged()
{
	StatusEffects.Charged = false;
}

void AEnemy::StartDOT()
{
	GetWorldTimerManager().SetTimer(ApplyDOTHandle, ApplyDOTDelegate, DOTInterval, true);
	DOTRemainingDuration = DOTDuration;
}

void AEnemy::ApplyDOT()
{
	DOTRemainingDuration -= DOTInterval;

	UGameplayStatics::ApplyDamage(
		this,
		DamageOnDOT,
		UGameplayStatics::GetPlayerController(this, 0),
		UGameplayStatics::GetPlayerCharacter(this, 0),
		UDamageType::StaticClass());

	if (DOTRemainingDuration <= 0)
	{
		GetWorldTimerManager().ClearTimer(ApplyDOTHandle);
	}
}
