// Fill out your copyright notice in the Description page of Project Settings.


#include "SnowGlobe.h"
#include "PlayerCharacter.h"
#include "BaseEntity.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

// Sets default values
ASnowGlobe::ASnowGlobe()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	TickDelegate.BindUObject(this, &ASnowGlobe::HandleTick);
}

// Called when the game starts or when spawned
void ASnowGlobe::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	
	RemainingTime = Lifetime;
	GetWorldTimerManager().SetTimer(TickHandle, TickDelegate, Interval, true);

	// DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Cyan, false, Lifetime);
}

// Called every frame
void ASnowGlobe::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ASnowGlobe::HandleTick()
{
	if (RemainingTime <= 0)
	{
		GetWorldTimerManager().ClearAllTimersForObject(this);
		Destroy();
		return;
	}
	
	RemainingTime -= Interval;

	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation(),
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(Radius),
		CollisionQueryParams
	);

	APlayerCharacter* Player;
	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Player = Cast<APlayerCharacter>(HitResult.GetActor());
		if (Player)
		{
			Player->AddMana(ManaPerTick);
			continue;
		}

		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (Enemy)
		{
			if (Enemy->StatusEffects.Burning)
			{
				UGameplayStatics::ApplyDamage(HitResult.GetActor(), DamagePerTick * IceburnMultiplier, PlayerController, PlayerCharacter, UDamageType::StaticClass());
				Enemy->ClearStatusEffects();
				return;
			}
			UGameplayStatics::ApplyDamage(HitResult.GetActor(), DamagePerTick, PlayerController, PlayerCharacter, UDamageType::StaticClass());
			Enemy->ApplyFrost();
		}
	}
}

