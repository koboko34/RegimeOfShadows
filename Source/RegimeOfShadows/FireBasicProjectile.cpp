// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBasicProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"


AFireBasicProjectile::AFireBasicProjectile()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFireBasicProjectile::OnCollision);
	Mesh->SetEnableGravity(false);
	
	ProjectileMovementComponent->ProjectileGravityScale = 0;
	ProjectileMovementComponent->bAutoActivate = false;
}

void AFireBasicProjectile::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
}

void AFireBasicProjectile::Release()
{
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetActorRotation(PlayerCharacter->GetCameraComponent()->GetForwardVector().Rotation());
	ProjectileMovementComponent->Velocity = PlayerCharacter->GetCameraComponent()->GetForwardVector() * ProjectileMovementComponent->InitialSpeed;
	ProjectileMovementComponent->Activate();
	bActive = true;
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
			PassthroughDamage,
			UGameplayStatics::GetPlayerController(this, 0),
			PlayerCharacter,
			UDamageType::StaticClass()
		);

		return;
	}

	Explode();
}

void AFireBasicProjectile::Explode()
{
	TArray<FHitResult> HitResults;
	HandleExplosion(HitResults, true, FColor::Red);

	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy)
			continue;

		Enemy->ApplyBurning();
	}

	PlayerCharacter->ResetFireQProj();

	Destroy();
}
