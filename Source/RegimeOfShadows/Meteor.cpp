// Armand Yilinkou, 2023


#include "Meteor.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilityComponent.h"
#include "Kismet/GameplayStatics.h"

AMeteor::AMeteor()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AMeteor::OnCollision);

	ProjectileMovementComponent->InitialSpeed = 600;
	ProjectileMovementComponent->ProjectileGravityScale = 0;
}

void AMeteor::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovementComponent->Velocity = ProjVelocity * ProjSpeed;

	float Factor = HeightOffset / -ProjVelocity.Z;
	FVector SpawnOffset = ProjVelocity * Factor;
	SpawnOffset *= -1;
	SpawnOffset.Z = 0;
	SetActorLocation(GetActorLocation() + SpawnOffset);
}

void AMeteor::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	TArray<FHitResult> HitResults;
	HandleExplosion(HitResults, false, FColor::Red);

	FTransform Transform = GetActorTransform();
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
