// Armand Yilinkou, 2023


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

	Destroy();
}
