// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "FireBasicProjectile.h"
#include "Enemy.h"
#include "Projectile.h"
#include "SnowGlobe.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);
}


void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set PlayerCharacter in AbilityComponent!"));
	}
}

void UAbilityComponent::BasicAttackFire()
{
	SpawnProjectile(FireAttackProj);
}

void UAbilityComponent::BasicAbilityFire()
{
	if (ActiveFireQProj && ActiveFireQProj->bActive)
	{
		ActiveFireQProj->Explode();
		return;
	}

	if (PlayerCharacter->GetMana() < FireQManaCost)
		return;

	PlayerCharacter->UseMana(FireQManaCost);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = PlayerCharacter;
	ActiveFireQProj = GetWorld()->SpawnActor<AFireBasicProjectile>(
		FireQProj,
		PlayerCharacter->Camera->GetComponentLocation() + PlayerCharacter->Camera->GetForwardVector() * 200,
		PlayerCharacter->Camera->GetComponentRotation(),
		Params
	);

	ActiveFireQProj->AttachToActor(PlayerCharacter, FAttachmentTransformRules::KeepWorldTransform);
}

void UAbilityComponent::BasicAbilityFireEnd()
{
	if (ActiveFireQProj)
	{
		ActiveFireQProj->Release();
	}
}

void UAbilityComponent::StrongAbilityFire()
{
	if (PlayerCharacter->GetMana() < FireEManaCost)
		return;

	PlayerCharacter->UseMana(FireEManaCost);
}

void UAbilityComponent::BasicAttackIce()
{
	SpawnProjectile(IceAttackProj);
}

void UAbilityComponent::BasicAbilityIce()
{
	if (PlayerCharacter->GetMana() < WaterAbilityManaCost)
		return;

	PlayerCharacter->UseMana(WaterAbilityManaCost);
	SpawnProjectile(WaterAbilityProj);
}

void UAbilityComponent::StrongAbilityIce()
{
	if (PlayerCharacter->GetMana() < SnowGlobeManaCost)
		return;

	PlayerCharacter->UseMana(SnowGlobeManaCost);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PlayerCharacter);
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		PlayerCharacter->GetActorLocation(),
		PlayerCharacter->GetActorLocation() + (PlayerCharacter->GetActorUpVector() * 200 * -1),
		ECollisionChannel::ECC_WorldStatic,
		CollisionQueryParams
	);

	DrawDebugLine(GetWorld(), PlayerCharacter->GetActorLocation(), PlayerCharacter->GetActorLocation() + (PlayerCharacter->GetActorUpVector() * 200 * -1), FColor::Red, false, 5.f);

	FVector SpawnLocation = PlayerCharacter->GetActorLocation();
	if (HitResult.bBlockingHit)
	{
		SpawnLocation = HitResult.ImpactPoint;
	}

	GetWorld()->SpawnActor<ASnowGlobe>(SnowGlobeClass, SpawnLocation, FRotator::ZeroRotator, Params);
}

void UAbilityComponent::BasicAttackElectric()
{
}

void UAbilityComponent::BasicAbilityElectric()
{
	if (PlayerCharacter->GetMana() < ElectricBasicManaCost)
		return;

	PlayerCharacter->UseMana(ElectricBasicManaCost);

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	FVector StartLocation = PlayerCharacter->Camera->GetComponentLocation();
	GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, StartLocation + PlayerCharacter->Camera->GetForwardVector() * 20000, ECollisionChannel::ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), StartLocation, OutHit.bBlockingHit ? OutHit.ImpactPoint : (StartLocation + PlayerCharacter->Camera->GetForwardVector() * 20000), FColor::Purple, false, 5.f);

	if (OutHit.bBlockingHit)
	{
		ElectricChainStart(OutHit.GetActor());
	}
}

void UAbilityComponent::StrongAbilityElectric()
{
	if (PlayerCharacter->GetMana() < ElectricStrongManaCost)
		return;

	PlayerCharacter->UseMana(ElectricStrongManaCost);
}

void UAbilityComponent::ElectricChainStart(AActor* HitActor)
{
	TArray<AActor*> HitActors;

	ElectricChainRecursive(HitActor, HitActors);
}

void UAbilityComponent::ElectricChainRecursive(AActor* HitActor, TArray<AActor*>& HitActors)
{
	HitActors.Add(HitActor);

	// apply effects to this HitActor
	AEnemy* Enemy = Cast<AEnemy>(HitActor);
	if (!Enemy)
		return;

	if (Enemy->StatusEffects.Burning)
	{
		UGameplayStatics::ApplyDamage(HitActor, ElectricChainDamage * OverloadedMultiplier, PlayerCharacter->GetController(), PlayerCharacter, UDamageType::StaticClass());
		Enemy->ClearStatusEffects();
		return;
	}

	UGameplayStatics::ApplyDamage(HitActor, ElectricChainDamage, PlayerCharacter->GetController(), PlayerCharacter, UDamageType::StaticClass());

	if (!Enemy->StatusEffects.Wet)
	{
		Enemy->ApplyCharged();
		return;
	}

	Enemy->ClearStatusEffects();

	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams Params;
	Params.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(HitActor);
	QueryParams.AddIgnoredActor(PlayerCharacter);
	GetWorld()->SweepMultiByObjectType(
		HitResults,
		HitActor->GetActorLocation(),
		HitActor->GetActorLocation(),
		FQuat::Identity,
		Params,
		FCollisionShape::MakeSphere(ElectricChainSpreadRadius),
		QueryParams);
	DrawDebugSphere(GetWorld(), HitActor->GetActorLocation(), ElectricChainSpreadRadius, 16, FColor::Purple, false, 5.f);

	int Count = 0;
	for (const FHitResult& HitResult : HitResults)
	{
		if (HitActors.Contains(HitResult.GetActor()))
			continue;

		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy)
			continue;

		if (!Enemy->StatusEffects.Wet)
			continue;

		ElectricChainRecursive(HitResult.GetActor(), HitActors);
		Enemy->ClearStatusEffects();

		Count++;

		if (Count > 1)
			return;
	}
}

AProjectile* UAbilityComponent::SpawnProjectile(TSubclassOf<AProjectile> ProjectileToSpawn)
{
	FActorSpawnParameters Params;
	Params.Owner = PlayerCharacter;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(
		ProjectileToSpawn,
		PlayerCharacter->Camera->GetComponentLocation() + PlayerCharacter->Camera->GetForwardVector() * 200,
		PlayerCharacter->Camera->GetComponentRotation(),
		Params);

	return Proj;
}