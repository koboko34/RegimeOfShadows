// Fill out your copyright notice in the Description page of Project Settings.


#include "FireAttackProjectile.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"

AFireAttackProjectile::AFireAttackProjectile()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFireAttackProjectile::OnCollision);
}

void AFireAttackProjectile::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	TArray<FHitResult> HitResults;
	HandleExplosion(HitResults, true, FColor::Red);

	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy)
			continue;

		Enemy->StartDOT();
	}

	Destroy();
}
