// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilityComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "FireBasicProjectile.h"
#include "Enemy.h"
#include "Projectile.h"
#include "SnowGlobe.h"
#include "ElectricPortal.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);

	BindDelegates();
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
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAttackFireHandle))
		return;

	TriggerCooldown(BasicAttackFireHandle, FireAttackCooldown);
	SpawnProjectile(FireAttackProj);
}

void UAbilityComponent::BasicAbilityFire()
{
	if (ActiveFireQProj && ActiveFireQProj->bActive)
	{
		ActiveFireQProj->Explode();
		return;
	}

	if (PlayerCharacter->GetMana() < FireQManaCost || GetWorld()->GetTimerManager().IsTimerActive(BasicAbilityFireHandle))
		return;

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
		TriggerCooldown(BasicAbilityFireHandle, FireQCooldown);
		PlayerCharacter->UseMana(FireQManaCost);
	}
}

void UAbilityComponent::StrongAbilityFire()
{
	if (PlayerCharacter->GetMana() < FireEManaCost || GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityFireHandle))
		return;

	TriggerCooldown(StrongAbilityFireHandle, FireECooldown);
	PlayerCharacter->UseMana(FireEManaCost);
}

void UAbilityComponent::BasicAttackIce()
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAttackIceHandle))
		return;

	TriggerCooldown(BasicAttackIceHandle, IceAttackCooldown);
	SpawnProjectile(IceAttackProj);
}

void UAbilityComponent::BasicAbilityIce()
{
	if (PlayerCharacter->GetMana() < WaterAbilityManaCost || GetWorld()->GetTimerManager().IsTimerActive(BasicAbilityIceHandle))
		return;

	TriggerCooldown(BasicAbilityIceHandle, WaterAbilityCooldown);
	PlayerCharacter->UseMana(WaterAbilityManaCost);
	SpawnProjectile(WaterAbilityProj);
}

void UAbilityComponent::StrongAbilityIce()
{
	if (PlayerCharacter->GetMana() < SnowGlobeManaCost || GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityIceHandle))
		return;

	TriggerCooldown(StrongAbilityIceHandle, SnowGlobeCooldown);
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
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAttackElectricHandle))
		return;

	TriggerCooldown(BasicAttackElectricHandle, ElectricAttackCooldown);
}

void UAbilityComponent::BasicAbilityElectric()
{
	if (PlayerCharacter->GetMana() < ElectricBasicManaCost || GetWorld()->GetTimerManager().IsTimerActive(BasicAbilityElectricHandle))
		return;

	TriggerCooldown(BasicAbilityElectricHandle, ElectricQCooldown);
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

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (!GetWorld()->GetTimerManager().IsTimerActive(PortalSpawnHandle))
	{
		FHitResult HitResult;
		FVector StartVector = PlayerCharacter->Camera->GetComponentLocation();
		FVector EndVector = StartVector + PlayerCharacter->Camera->GetForwardVector() * PortalSpawnTraceDistance;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor(PlayerCharacter);
		GetWorld()->LineTraceSingleByChannel(HitResult, StartVector, EndVector, ECollisionChannel::ECC_WorldStatic, Params);

		if (!HitResult.bBlockingHit)
		{
			return;
		}

		FirstPortalSpawnLocation = HitResult.ImpactPoint;
		PortalMarker = GetWorld()->SpawnActor<AActor>(
			PortalMarkerClass,
			FirstPortalSpawnLocation,
			FRotator::ZeroRotator,
			SpawnParams
		);

		GetWorld()->GetTimerManager().SetTimer(PortalSpawnHandle, PortalSpawnDelegate, PortalSpawnTimeout, false);
		return;
	}

	FHitResult HitResult;
	FVector StartVector = PlayerCharacter->Camera->GetComponentLocation();
	FVector EndVector = StartVector + PlayerCharacter->Camera->GetForwardVector() * PortalSpawnTraceDistance;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	GetWorld()->LineTraceSingleByChannel(HitResult, StartVector, EndVector, ECollisionChannel::ECC_WorldStatic, Params);

	if (!HitResult.bBlockingHit)
	{
		return;
	}

	TriggerCooldown(StrongAbilityElectricHandle, ElectricECooldown);
	PlayerCharacter->UseMana(ElectricStrongManaCost);
	GetWorld()->GetTimerManager().ClearTimer(PortalSpawnHandle);
	
	FRotator FirstPortalSpawnRotation = (HitResult.ImpactPoint - FirstPortalSpawnLocation).Rotation();
	AElectricPortal* FirstPortal = GetWorld()->SpawnActor<AElectricPortal>(
		ElectricPortalClass,
		FirstPortalSpawnLocation,
		FirstPortalSpawnRotation,
		SpawnParams
	);

	FRotator SecondPortalSpawnRotation = (FirstPortalSpawnLocation - HitResult.ImpactPoint).Rotation();
	AElectricPortal* SecondPortal = GetWorld()->SpawnActor<AElectricPortal>(
		ElectricPortalClass,
		HitResult.ImpactPoint,
		SecondPortalSpawnRotation,
		SpawnParams
	);

	ClearPortalMarker();
	FirstPortal->Init(SecondPortal, this);
}

void UAbilityComponent::ElectricChainStart(AActor* HitActor)
{
	TArray<AActor*> HitActors;

	ElectricChainRecursive(HitActor, HitActors);
}

void UAbilityComponent::ElectricChainRecursive(AActor* HitActor, TArray<AActor*>& HitActors)
{
	HitActors.Add(HitActor);

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

void UAbilityComponent::CancelPortalSpawn()
{
	ClearPortalMarker();
}

void UAbilityComponent::ClearPortalMarker()
{
	if (!PortalMarker)
		return;

	PortalMarker->Destroy();
	PortalMarker = nullptr;
}

void UAbilityComponent::TriggerCooldown(FTimerHandle& AbilityHandle, float Duration)
{
	GetWorld()->GetTimerManager().SetTimer(AbilityHandle, Duration, false);
}

void UAbilityComponent::BasicAttackFireCooldown()
{
}

void UAbilityComponent::BasicAbilityFireCooldown()
{
}

void UAbilityComponent::StrongAbilityFireCooldown()
{
}

void UAbilityComponent::BasicAttackIceCooldown()
{
}

void UAbilityComponent::BasicAbilityIceCooldown()
{
}

void UAbilityComponent::StrongAbilityIceCooldown()
{
}

void UAbilityComponent::BasicAttackElectricCooldown()
{
}

void UAbilityComponent::BasicAbilityElectricCooldown()
{
}

void UAbilityComponent::StrongAbilityElectricCooldown()
{
}

void UAbilityComponent::BindDelegates()
{
	PortalSpawnDelegate.BindUObject(this, &UAbilityComponent::CancelPortalSpawn);
	
	BasicAttackFireDelegate.BindUObject(this, &UAbilityComponent::BasicAttackFireCooldown);
	BasicAbilityFireDelegate.BindUObject(this, &UAbilityComponent::BasicAbilityFireCooldown);
	StrongAbilityFireDelegate.BindUObject(this, &UAbilityComponent::StrongAbilityFireCooldown);
	BasicAttackIceDelegate.BindUObject(this, &UAbilityComponent::BasicAttackIceCooldown);
	BasicAbilityIceDelegate.BindUObject(this, &UAbilityComponent::BasicAbilityIceCooldown);
	StrongAbilityIceDelegate.BindUObject(this, &UAbilityComponent::StrongAbilityIceCooldown);
	BasicAttackElectricDelegate.BindUObject(this, &UAbilityComponent::BasicAttackElectricCooldown);
	BasicAbilityElectricDelegate.BindUObject(this, &UAbilityComponent::BasicAbilityElectricCooldown);
	StrongAbilityElectricDelegate.BindUObject(this, &UAbilityComponent::StrongAbilityElectricCooldown);
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

float UAbilityComponent::GetFireAttackCooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAttackFireHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(BasicAttackFireHandle);

	return 0.f;
}

float UAbilityComponent::GetFireQCooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAbilityFireHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(BasicAbilityFireHandle);

	return 0.f;
}

float UAbilityComponent::GetFireECooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityFireHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(StrongAbilityFireHandle);

	return 0.f;
}

float UAbilityComponent::GetIceAttackCooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAttackIceHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(BasicAttackIceHandle);

	return 0.f;
}

float UAbilityComponent::GetIceQCooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAbilityIceHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(BasicAbilityIceHandle);

	return 0.f;
}

float UAbilityComponent::GetIceECooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityIceHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(StrongAbilityIceHandle);

	return 0.f;
}

float UAbilityComponent::GetElectricAttackCooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAttackElectricHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(BasicAttackElectricHandle);

	return 0.f;
}

float UAbilityComponent::GetElectricQCooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(BasicAbilityElectricHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(BasicAbilityElectricHandle);

	return 0.f;
}

float UAbilityComponent::GetElectricECooldown() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityElectricHandle))
		return GetWorld()->GetTimerManager().GetTimerRemaining(StrongAbilityElectricHandle);

	return 0.f;
}
