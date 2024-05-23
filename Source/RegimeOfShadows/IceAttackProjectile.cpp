// Armand Yilinkou, 2023


#include "IceAttackProjectile.h"
#include "Components/SphereComponent.h"
#include "Enemy.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"

AIceAttackProjectile::AIceAttackProjectile()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AIceAttackProjectile::OnCollision);
}

void AIceAttackProjectile::BeginPlay()
{
	Super::BeginPlay();

	Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (!Player)
		UE_LOG(LogTemp, Warning, TEXT("IceProjectile couldn't set Player!"));
}

void AIceAttackProjectile::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	TArray<FHitResult> HitResults;
	HandleExplosion(HitResults, true, FColor::Cyan);

	if (IceSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, IceSound, GetActorLocation(), IceSoundVolume);
	}

	TArray<AActor*> HitEnemies;
	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy || HitEnemies.Contains(Enemy))
			continue;

		HitEnemies.Add(Enemy);
		Player->AddMana(ManaPerTarget);
	}

	Destroy();
}
