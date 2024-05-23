// Armand Yilinkou, 2023


#include "FireBasicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "AbilityComponent.h"

AFireBasicProjectile::AFireBasicProjectile()
{
	PrimaryActorTick.bCanEverTick = true;
	SetActorTickEnabled(true);
	
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFireBasicProjectile::OnCollision);
	Mesh->SetEnableGravity(false);
	
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->bAutoActivate = false;

	GrowthDelegate.BindUObject(this, &AFireBasicProjectile::GrowthFinish);
}

void AFireBasicProjectile::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	InitialExplosionRadius = ExplosionRadius;
	InitialSpeed = ProjectileMovementComponent->InitialSpeed;
	InitialScale = GetActorScale().X;

	GetWorldTimerManager().SetTimer(GrowthHandle, GrowthDelegate, GrowDuration / PlayerCharacter->GetSpellSpeed(), false);
}

void AFireBasicProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Grow();
}

void AFireBasicProjectile::Release()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorRotation(PlayerCharacter->GetCameraComponent()->GetForwardVector().Rotation());
	ProjectileMovementComponent->Velocity = PlayerCharacter->GetCameraComponent()->GetForwardVector() * ProjectileMovementComponent->InitialSpeed;
	ProjectileMovementComponent->Activate();
	bActive = true;

	if (GetWorldTimerManager().IsTimerActive(GrowthHandle))
		GetWorldTimerManager().ClearTimer(GrowthHandle);

	SetActorTickEnabled(false);
}

void AFireBasicProjectile::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!bActive || HitActors.Contains(OtherActor))
		return;

	AEnemy* Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy)
	{
		HitActors.Add(OtherActor);

		UGameplayStatics::ApplyDamage(
			OtherActor,
			PassthroughDamage * GrowthFactor,
			UGameplayStatics::GetPlayerController(this, 0),
			PlayerCharacter,
			UDamageType::StaticClass()
		);

		return;
	}

	Explode();
}

void AFireBasicProjectile::GrowthFinish()
{
	SetActorTickEnabled(false);

	GrowthFactor = 2.f;
	ExplosionRadius = InitialExplosionRadius * GrowthFactor;
	ProjectileMovementComponent->InitialSpeed = InitialSpeed * GrowthFactor;
	SetActorScale3D(FVector(TargetScale, TargetScale, TargetScale));
}

void AFireBasicProjectile::Grow()
{
	float Time = GetWorldTimerManager().GetTimerElapsed(GrowthHandle);
	GrowthFactor = 1.f + (Time / GrowDuration);

	ExplosionRadius = InitialExplosionRadius * GrowthFactor;
	float NewScale = FMath::Lerp(InitialScale, TargetScale, Time / (GrowDuration / PlayerCharacter->GetSpellSpeed()));
	SetActorScale3D(FVector(NewScale, NewScale, NewScale));
}

void AFireBasicProjectile::Explode()
{
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

	TArray<AActor*> HitEnemies;
	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy || HitEnemies.Contains(Enemy))
			continue;

		HitEnemies.Add(Enemy);
		float AdjustedDamageToDeal = Enemy->StatusEffects.Charged ? DamageToDeal * GrowthFactor * PlayerCharacter->GetAbilityComponent()->OverloadedMultiplier
			: DamageToDeal * GrowthFactor;

		UGameplayStatics::ApplyDamage(
			Enemy,
			AdjustedDamageToDeal,
			PlayerCharacter->GetController(),
			PlayerCharacter,
			UDamageType::StaticClass()
		);

		Enemy->ApplyBurning();
	}

	PlayerCharacter->ResetFireQProj();

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
