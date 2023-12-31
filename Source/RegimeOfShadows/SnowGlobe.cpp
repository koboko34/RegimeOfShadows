// Armand Yilinkou, 2023


#include "SnowGlobe.h"
#include "PlayerCharacter.h"
#include "BaseEntity.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

ASnowGlobe::ASnowGlobe()
{
	PrimaryActorTick.bCanEverTick = true;

	TickDelegate.BindUObject(this, &ASnowGlobe::HandleTick);
}

void ASnowGlobe::BeginPlay()
{
	Super::BeginPlay();

	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
	PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	
	RemainingTime = Lifetime;
	GetWorldTimerManager().SetTimer(TickHandle, TickDelegate, Interval, true);

	// DrawDebugSphere(GetWorld(), GetActorLocation(), Radius, 32, FColor::Cyan, false, Lifetime);
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
		if (!Enemy->GetIsAlive())
			continue;

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

