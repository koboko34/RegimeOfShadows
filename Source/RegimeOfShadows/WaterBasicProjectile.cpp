// Armand Yilinkou, 2023


#include "WaterBasicProjectile.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"

AWaterBasicProjectile::AWaterBasicProjectile()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AWaterBasicProjectile::OnCollision);
}

void AWaterBasicProjectile::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	TArray<FHitResult> HitResults;
	HandleExplosion(HitResults, true, FColor::Blue);

	if (WaterSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, WaterSound, GetActorLocation(), WaterSoundVolume);
	}

	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy)
			continue;

		Enemy->ApplyWet();
	}

	Destroy();
}
