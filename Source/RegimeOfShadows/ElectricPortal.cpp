// Armand Yilinkou, 2023


#include "ElectricPortal.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "AbilityComponent.h"

AElectricPortal::AElectricPortal()
{
	PrimaryActorTick.bCanEverTick = false;
	SetActorTickEnabled(false);

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	Mesh->bReceivesDecals = false;
	ZapTarget = CreateDefaultSubobject<USceneComponent>("ZapTarget");
	ZapTarget->SetupAttachment(RootComponent);

	PulseDelegate.BindUObject(this, &AElectricPortal::Pulse);
}

void AElectricPortal::BeginPlay()
{
	Super::BeginPlay();
	
	PlayerActor = Cast<AActor>(UGameplayStatics::GetPlayerCharacter(this, 0));
	PlayerController = UGameplayStatics::GetPlayerController(this, 0);
}

void AElectricPortal::Init(AElectricPortal* Portal, UAbilityComponent* InAbilityComponent)
{
	PartnerPortal = Portal;
	AbilityComponent = InAbilityComponent;
	
	if (Portal->GetPartnerPortal())
		return;

	RemainingTime = AbilityComponent->PortalDuration;
	GetWorldTimerManager().SetTimer(PulseHandle, PulseDelegate, 1.f, true);
}

void AElectricPortal::Pulse()
{
	RemainingTime -= 1.f;
	if (RemainingTime <= 0)
	{
		Sweep();
		PartnerPortal->Destroy();
		Destroy();
		return;
	}

	Sweep();
	GetWorldTimerManager().SetTimer(PulseHandle, PulseDelegate, 1.f, true);
}

void AElectricPortal::Sweep()
{
	TArray<FHitResult> HitResults;
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_Pawn);
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(PlayerActor);
	GetWorld()->SweepMultiByObjectType(
		HitResults,
		GetActorLocation() + FVector(0, 0, SweepZOffset),
		PartnerPortal->GetActorLocation() + FVector(0, 0, SweepZOffset),
		FQuat::Identity,
		ObjectQueryParams,
		FCollisionShape::MakeSphere(AbilityComponent->PortalSweepRadius),
		CollisionQueryParams
	);

	if (LaserZapSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, LaserZapSound, GetActorLocation(), LaserZapVolume);
		UGameplayStatics::PlaySoundAtLocation(this, LaserZapSound, PartnerPortal->GetActorLocation(), LaserZapVolume);
	}

	if (ElectricBeamSystem)
	{
		UNiagaraComponent* ElectricBeam = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this,
			ElectricBeamSystem,
			ZapTarget->GetComponentLocation(),
			FRotator::ZeroRotator,
			FVector(1.f),
			true,
			false);
		ElectricBeam->SetVectorParameter("BeamEnd", PartnerPortal->GetZapTargetLocation());
		ElectricBeam->Activate();
	}

	TArray<AActor*> HitEnemies;
	AEnemy* Enemy;
	for (const FHitResult& HitResult : HitResults)
	{
		Enemy = Cast<AEnemy>(HitResult.GetActor());
		if (!Enemy || HitEnemies.Contains(Enemy))
		{
			continue;
		}

		HitEnemies.Add(Enemy);
		if (Enemy->StatusEffects.Burning)
		{
			UGameplayStatics::ApplyDamage(
				Enemy,
				AbilityComponent->PortalDamage * AbilityComponent->OverloadedMultiplier,
				PlayerController, PlayerActor,
				UDamageType::StaticClass()
			);

			Enemy->ClearStatusEffects();
			continue;
		}

		UGameplayStatics::ApplyDamage(
			Enemy,
			AbilityComponent->PortalDamage,
			PlayerController, PlayerActor,
			UDamageType::StaticClass()
		);

		Enemy->ApplyCharged();
	}
}
