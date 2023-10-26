// Fill out your copyright notice in the Description page of Project Settings.


#include "FireAttackProjectile.h"
#include "Components/SphereComponent.h"
#include "BaseEntity.h"
#include "Kismet/GameplayStatics.h"

AFireAttackProjectile::AFireAttackProjectile()
{
	SphereCollider->OnComponentBeginOverlap.AddDynamic(this, &AFireAttackProjectile::OnCollision);
}

void AFireAttackProjectile::OnCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	UE_LOG(LogTemp, Warning, TEXT("Collided with %s"), *OtherActor->GetName());
	
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(UGameplayStatics::GetPlayerCharacter(this, 0));
	UGameplayStatics::ApplyRadialDamage(this, DamageToDeal, GetActorLocation(), 100.f, UDamageType::StaticClass(), ActorsToIgnore, this, UGameplayStatics::GetPlayerController(this, 0));
	DrawDebugSphere(GetWorld(), GetActorLocation(), 100.f, 16, FColor::Red, false, 5.f);
	Destroy();
}
