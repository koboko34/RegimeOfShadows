// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"

AEnemy::AEnemy()
{
	CalculateKillExp();

	ClearBurningDelegate.BindUObject(this, &AEnemy::ClearBurning);
	ClearWetDelegate.BindUObject(this, &AEnemy::ClearWet);
	ClearFrostDelegate.BindUObject(this, &AEnemy::ClearFrost);
	ClearChargedDelegate.BindUObject(this, &AEnemy::ClearCharged);

	ApplyDOTDelegate.BindUObject(this, &AEnemy::ApplyDOT);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	ClearStatusEffects();
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
	ClearStatusEffects();
	StatusEffects.Burning = true;
	StartDOT();
	ApplyBurningMaterial();
	GetWorldTimerManager().SetTimer(ClearBurningHandle, ClearBurningDelegate, StatusDuration, false);
}

void AEnemy::ApplyWet()
{
	ClearStatusEffects();
	StatusEffects.Wet = true;
	ApplyWetMaterial();
	GetWorldTimerManager().SetTimer(ClearWetHandle, ClearWetDelegate, StatusDuration, false);
}

void AEnemy::ApplyFrost()
{
	ClearStatusEffects();
	StatusEffects.Frost = true;
	ApplyFrostMaterial();
	GetWorldTimerManager().SetTimer(ClearFrostHandle, ClearFrostDelegate, StatusDuration, false);
}

void AEnemy::ApplyCharged()
{
	ClearStatusEffects();
	StatusEffects.Charged = true;
	StartDOT();
	ApplyChargedMaterial();
	GetWorldTimerManager().SetTimer(ClearChargedHandle, ClearChargedDelegate, StatusDuration, false);
}

void AEnemy::ClearStatusEffects()
{
	StatusEffects.Burning = false;
	StatusEffects.Wet = false;
	StatusEffects.Frost = false;
	StatusEffects.Charged = false;
	RemoveStatusMaterial();

	GetWorldTimerManager().ClearAllTimersForObject(this);
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
	RemoveStatusMaterial();
	StatusEffects.Burning = false;
}

void AEnemy::ClearWet()
{
	RemoveStatusMaterial();
	StatusEffects.Wet = false;
}

void AEnemy::ClearFrost()
{
	RemoveStatusMaterial();
	StatusEffects.Frost = false;
}

void AEnemy::ClearCharged()
{
	RemoveStatusMaterial();
	StatusEffects.Charged = false;
}

void AEnemy::StartDOT()
{
	float DOTInterval = StatusEffects.Burning ? FireDOTInterval : ElectricDOTInterval;
	GetWorldTimerManager().SetTimer(ApplyDOTHandle, ApplyDOTDelegate, DOTInterval, true);
	DOTRemainingDuration = StatusDuration;
}

void AEnemy::ApplyDOT()
{
	if (DOTRemainingDuration <= 0 || (!StatusEffects.Burning && !StatusEffects.Charged))
	{
		GetWorldTimerManager().ClearTimer(ApplyDOTHandle);
		return;
	}
	
	float DOTInterval = StatusEffects.Burning ? FireDOTInterval : ElectricDOTInterval;
	int DamageOnDOT = StatusEffects.Burning ? FireDamageOnDOT : ElectricDamageOnDOT;
	DOTRemainingDuration -= DOTInterval;

	UGameplayStatics::ApplyDamage(
		this,
		DamageOnDOT,
		UGameplayStatics::GetPlayerController(this, 0),
		UGameplayStatics::GetPlayerCharacter(this, 0),
		UDamageType::StaticClass());
}
