// Armand Yilinkou, 2023


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
	TArray<FHitResult> HitResults;
	HandleExplosion(HitResults, false, FColor::Red);

	FTransform Transform = GetActorTransform();
	Transform.MultiplyScale3D(FVector(2));
	if (ExplosionParticleSystem)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionParticleSystem, Transform);
	}
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, ExplosionSound, GetActorLocation(), ExplosionVolume);
	}

	Destroy();
}
