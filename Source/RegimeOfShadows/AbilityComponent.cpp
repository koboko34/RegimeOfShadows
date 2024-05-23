// Armand Yilinkou, 2023


#include "AbilityComponent.h"
#include "PlayerCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Camera/CameraComponent.h"
#include "FireBasicProjectile.h"
#include "Enemy.h"
#include "Projectile.h"
#include "SnowGlobe.h"
#include "ElectricPortal.h"
#include "Decal.h"
#include "Components/DecalComponent.h"
#include "NiagaraComponent.h" 
#include "NiagaraFunctionLibrary.h"

UAbilityComponent::UAbilityComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetComponentTickEnabled(false);

	PlayerCharacter = Cast<APlayerCharacter>(GetOwner());
	if (!PlayerCharacter)
	{
		UE_LOG(LogTemp, Warning, TEXT("Failed to set PlayerCharacter in AbilityComponent!"));
		return;
	}

	SpellSpeed = &PlayerCharacter->SpellSpeed;

	MeteorShowerDelegate.BindUObject(this, &UAbilityComponent::MeteorShower);
	MeteorShowerCastDelegate.BindUObject(this, &UAbilityComponent::MeteorShowerCast);
	PortalSpawnDelegate.BindUObject(this, &UAbilityComponent::CancelPortalSpawn);
	SweepWeaponDelegate.BindUObject(this, &UAbilityComponent::ElectricBasicSweep);
}

void UAbilityComponent::BeginPlay()
{
	Super::BeginPlay();
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

	if (bCancelAbility)
	{
		if (MeteorDecal)
		{
			MeteorDecal->Destroy();
			MeteorDecal = nullptr;
		}
		return;
	}

	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(PlayerCharacter);
	FVector StartLocation = PlayerCharacter->Camera->GetComponentLocation();
	FVector EndLocation = StartLocation + PlayerCharacter->Camera->GetForwardVector() * 20000;
	GetWorld()->LineTraceSingleByChannel(
		OutHit,
		StartLocation,
		EndLocation,
		ECollisionChannel::ECC_Visibility, Params
	);
	
	if (!OutHit.bBlockingHit)
		return;

	MeteorLocation = OutHit.ImpactPoint;
	if (!MeteorDecal)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		MeteorDecal = GetWorld()->SpawnActor<ADecal>(MeteorDecalClass, MeteorLocation, FRotator(90, FMath::RandRange(0, 360), 0), SpawnParams);
		if (MeteorDecal)
			MeteorDecal->Decal->DecalSize = FVector(DecalDepth, MeteorSpawnRadius, MeteorSpawnRadius);
		
		return;
	}

	MeteorDecal->SetActorLocation(MeteorLocation);
}

void UAbilityComponent::StrongAbilityFireEnd()
{
	if (bCancelAbility)
	{
		bCancelAbility = false;
		return;
	}
	
	if (GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityFireHandle) || PlayerCharacter->GetMana() < FireEManaCost || !MeteorDecal)
		return;
		
	GetWorld()->GetTimerManager().SetTimer(MeteorShowerCastHandle, MeteorShowerCastDelegate, MeteorShowerCastDuration, false);
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
	if (PlayerCharacter->GetMana() < SnowGlobeManaCost || GetWorld()->GetTimerManager().IsTimerActive(StrongAbilityIceHandle) ||
		PlayerCharacter->GetStrongAbilityDown())
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

	PlayerCharacter->WeaponSwing();
	
	GetWorld()->GetTimerManager().SetTimer(SweepWeaponHandle, SweepWeaponDelegate, 0.1f, false);
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
	float MaxLength = 20000;
	FVector StartLocation = PlayerCharacter->Camera->GetComponentLocation();
	FVector EndLocation = StartLocation + PlayerCharacter->Camera->GetForwardVector() * MaxLength;
	GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, EndLocation, ECollisionChannel::ECC_Visibility, Params);

	if (OutHit.bBlockingHit)
	{
		ElectricChainStart(OutHit.GetActor());
	}

	if (LaserZapSound)
	{
		UGameplayStatics::PlaySound2D(this, LaserZapSound, LaserZapVolume);
	}

	if (ElectricBeamSystem)
	{
		UNiagaraComponent* ElectricBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ElectricBeamSystem,
			PlayerCharacter->Camera->GetComponentLocation() + FVector(0, 0, -20),
			FRotator::ZeroRotator,
			FVector(1.f),
			true,
			false
		);
		FVector BeamEnd = OutHit.bBlockingHit ? OutHit.ImpactPoint : EndLocation;
		ElectricBeam->SetVectorParameter("BeamEnd", BeamEnd);
		ElectricBeam->Activate();
	}
}

void UAbilityComponent::StrongAbilityElectric()
{
	if (PlayerCharacter->GetMana() < ElectricStrongManaCost || PlayerCharacter->GetStrongAbilityDown())
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

void UAbilityComponent::CancelAbilityCast()
{
	if (!GetWorld()->GetTimerManager().IsTimerActive(MeteorShowerCastHandle))
		return;

	GetWorld()->GetTimerManager().ClearTimer(MeteorShowerCastHandle);
	if (MeteorDecal)
	{
		MeteorDecal->Destroy();
		MeteorDecal = nullptr;
	}
}

void UAbilityComponent::CancelAbility()
{
	bCancelAbility = true;
}

void UAbilityComponent::MeteorShowerCast()
{
	MeteorShowerTimeRemaining = MeteorShowerDuration;
	GetWorld()->GetTimerManager().SetTimer(MeteorShowerHandle, MeteorShowerDelegate, MeteorSpawnInterval, true);

	TriggerCooldown(StrongAbilityFireHandle, FireECooldown);
	PlayerCharacter->UseMana(FireEManaCost);
}

void UAbilityComponent::MeteorShower()
{
	FVector RandomOffset = FVector(FMath::RandRange(-MeteorSpawnRadius, MeteorSpawnRadius), FMath::RandRange(-MeteorSpawnRadius, MeteorSpawnRadius), MeteorShowerHeightOffset);
	FVector SpawnLocation = MeteorLocation + RandomOffset;
	FActorSpawnParameters Params;
	Params.Owner = PlayerCharacter;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	GetWorld()->SpawnActor<AProjectile>(
		MeteorClass,
		SpawnLocation,
		FRotator::ZeroRotator,
		Params
	);

	if (MeteorShowerTimeRemaining <= 0)
	{
		GetWorld()->GetTimerManager().ClearTimer(MeteorShowerHandle);
		MeteorDecal->Destroy();
		MeteorDecal = nullptr;
		return;
	}

	MeteorShowerTimeRemaining -= MeteorSpawnInterval;
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

	Enemy->ApplyCharged();

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

		Count++;

		if (Count > 1)
			return;
	}
}

void UAbilityComponent::ElectricBasicSweep()
{
	TArray<FHitResult> HitResults;
	FVector AttackLocation = PlayerCharacter->Camera->GetComponentLocation() + PlayerCharacter->Camera->GetForwardVector() * ElectricAttackDistance;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PlayerCharacter);
	FQuat Quat = (PlayerCharacter->Camera->GetForwardVector().Rotation() + FRotator(0, 0, 90)).Quaternion();
	GetWorld()->SweepMultiByObjectType(
		HitResults,
		AttackLocation,
		AttackLocation,
		Quat,
		ObjectQueryParams,
		FCollisionShape::MakeCapsule(ElectricAttackRadius, ElectricAttackHalfHeight),
		CollisionQueryParams
	);

	DrawDebugCapsule(GetWorld(), AttackLocation, ElectricAttackHalfHeight, ElectricAttackRadius, Quat, FColor::Purple, false, 2.f);

	for (const FHitResult& HitResult : HitResults)
	{
		UGameplayStatics::ApplyDamage(
			HitResult.GetActor(),
			ElectricAttackDamage,
			PlayerCharacter->GetController(),
			PlayerCharacter,
			UDamageType::StaticClass()
		);
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
	GetWorld()->GetTimerManager().SetTimer(AbilityHandle, Duration / PlayerCharacter->GetCooldownFactor(), false);
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

float UAbilityComponent::GetCastProgress() const
{
	if (GetWorld()->GetTimerManager().IsTimerActive(MeteorShowerCastHandle))
	{
		float Elapsed = GetWorld()->GetTimerManager().GetTimerElapsed(MeteorShowerCastHandle);	
		return Elapsed / (GetWorld()->GetTimerManager().GetTimerRemaining(MeteorShowerCastHandle) + Elapsed);
	}
	
	return -1.f;
}
