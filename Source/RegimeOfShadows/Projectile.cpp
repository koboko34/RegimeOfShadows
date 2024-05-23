// Armand Yilinkou, 2023


#include "Projectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh->bReceivesDecals = false;

	SphereCollider = CreateDefaultSubobject<USphereComponent>("SphereCollider");
	SphereCollider->SetupAttachment(Mesh);
	SphereCollider->SetCollisionObjectType(ECollisionChannel::ECC_GameTraceChannel1);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
}

void AProjectile::HandleExplosion(TArray<FHitResult>& HitResultsOut, bool bDrawDebug, FColor Colour, float DamageMultiplier)
{
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(this, 0);
	AController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);

	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PlayerCharacter);

	GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation(),
		GetActorLocation(),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(ExplosionRadius),
		CollisionQueryParams);

	HitResultsOut = HitResults;
	if (bDrawDebug)
		DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 16, Colour, false, 5.f);

	TArray<AActor*> HitEnemies;
	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy || !Enemy->GetIsAlive() || HitEnemies.Contains(Enemy))
			continue;

		UGameplayStatics::ApplyDamage(
			Enemy,
			DamageToDeal * DamageMultiplier,
			PlayerController,
			PlayerCharacter,
			UDamageType::StaticClass());

		HitEnemies.Add(Enemy);
	}
}
