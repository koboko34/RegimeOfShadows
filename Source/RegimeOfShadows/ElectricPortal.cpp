// Fill out your copyright notice in the Description page of Project Settings.


#include "ElectricPortal.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "AbilityComponent.h"

AElectricPortal::AElectricPortal()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->bReceivesDecals = false;

	PulseDelegate.BindUObject(this, &AElectricPortal::Pulse);
}

void AElectricPortal::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void AElectricPortal::Init(AElectricPortal* Portal, UAbilityComponent* InAbilityComponent)
{
	PartnerPortal = Portal;
	AbilityComponent = InAbilityComponent;
	
	if (Portal->GetPartnerPortal())
		return;

	RemainingTime = AbilityComponent->PortalDuration;
	GetWorldTimerManager().SetTimer(PulseHandle, PulseDelegate, 1.f, true);
}

void AElectricPortal::Pulse()
{
	RemainingTime -= 1.f;
	if (RemainingTime <= 0)
	{
		Sweep();
		PartnerPortal->Destroy();
		Destroy();
		return;
	}

	Sweep();
	GetWorldTimerManager().SetTimer(PulseHandle, PulseDelegate, 1.f, true);
}

void AElectricPortal::Sweep()
{
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PlayerActor);
	GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation() + FVector(0, 0, SweepZOffset),
		PartnerPortal->GetActorLocation() + FVector(0, 0, SweepZOffset),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(AbilityComponent->PortalSweepRadius),
		CollisionQueryParams
	);

	DrawDebugCylinder(
		GetWorld(),
		GetActorLocation() + FVector(0, 0, SweepZOffset),
		PartnerPortal->GetActorLocation() + FVector(0, 0, SweepZOffset),
		AbilityComponent->PortalSweepRadius,
		16,
		FColor::Purple,
		false,
		1.f
	);

	AEnemy* Enemy;
	AController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy)
		{
			continue;
		}

		if (Enemy->StatusEffects.Burning)
		{
			UGameplayStatics::ApplyDamage(
				Enemy,
				AbilityComponent->PortalDamage * AbilityComponent->OverloadedMultiplier,
				PlayerController, PlayerActor,
				UDamageType::StaticClass()
			);

			Enemy->ClearStatusEffects();
			continue;
		}

		UGameplayStatics::ApplyDamage(
			Enemy,
			AbilityComponent->PortalDamage,
			PlayerController, PlayerActor,
			UDamageType::StaticClass()
		);

		Enemy->ApplyCharged();
	}
}
