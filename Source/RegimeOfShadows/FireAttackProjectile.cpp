// Fill out your copyright notice in the Description page of Project Settings.


#include "FireAttackProjectile.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AFireAttackProjectile::AFireAttackProjectile()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFireAttackProjectile::OnCollision);
}

void AFireAttackProjectile::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
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

	DrawDebugSphere(GetWorld(), GetActorLocation(), ExplosionRadius, 16, FColor::Red, false, 5.f);

	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy)
			continue;

		UGameplayStatics::ApplyDamage(
			Enemy,
			DamageToDeal,
			PlayerController,
			PlayerCharacter,
			UDamageType::StaticClass());

		Enemy->StartDOT();
	}

	Destroy();
}
