// Armand Yilinkou, 2023


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"


AEnemy::AEnemy()
{
	AttackPoint = CreateDefaultSubobject<USceneComponent>("AttackPoint");
	AttackPoint->SetupAttachment(RootComponent);

	PawnSensingComponent = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComponent");
	PawnSensingComponent->bHearNoises = true;
	PawnSensingComponent->bSeePawns = false;
	PawnSensingComponent->bOnlySensePlayers = true;
	PawnSensingComponent->HearingThreshold = HearingRange;
	PawnSensingComponent->LOSHearingThreshold = HearingRange;

	PawnSensingComponent->OnHearNoise.AddDynamic(this, &AEnemy::OnHearNoise);
	RangeCheckDelegate.BindUObject(this, &AEnemy::RangeCheck);
	
	CalculateKillExp();

	SetRootComponent(GetCapsuleComponent());
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ClearBurningDelegate.BindUObject(this, &AEnemy::ClearBurning);
	ClearWetDelegate.BindUObject(this, &AEnemy::ClearWet);
	ClearFrostDelegate.BindUObject(this, &AEnemy::ClearFrost);
	ClearChargedDelegate.BindUObject(this, &AEnemy::ClearCharged);

	ApplyDOTDelegate.BindUObject(this, &AEnemy::ApplyDOT);

	DestroyDelegate.BindUObject(this, &AEnemy::DestroyAfterDeath);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	ClearStatusEffects();

	AAIController* EnemyController = Cast<AAIController>(GetController());
	BlackboardComponent = EnemyController->GetBlackboardComponent();
	BlackboardComponent->SetValueAsBool(FName("bSensesPlayer"), false);
	InitialLocation = GetActorLocation();
	BlackboardComponent->SetValueAsVector(FName("InitialLocation"), InitialLocation);

	GetWorldTimerManager().SetTimer(RangeCheckHandle, RangeCheckDelegate, 1.f, true);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	UE_LOG(LogTemp, Warning, TEXT("Enemy Health: %i"), Health);
	if (Health <= 0)
	{
		Death();
	}

	return DamageAmount;
}

void AEnemy::ApplyBurning()
{
	ClearStatusEffects();
	StatusEffects.Burning = true;
	StartDOT();
	ApplyBurningMaterial();
	GetWorldTimerManager().SetTimer(ClearBurningHandle, ClearBurningDelegate, StatusDuration, false);
}

void AEnemy::ApplyWet()
{
	ClearStatusEffects();
	StatusEffects.Wet = true;
	ApplyWetMaterial();
	GetWorldTimerManager().SetTimer(ClearWetHandle, ClearWetDelegate, StatusDuration, false);
}

void AEnemy::ApplyFrost()
{
	ClearStatusEffects();
	StatusEffects.Frost = true;
	ApplyFrostMaterial();
	GetWorldTimerManager().SetTimer(ClearFrostHandle, ClearFrostDelegate, StatusDuration, false);
}

void AEnemy::ApplyCharged()
{
	ClearStatusEffects();
	StatusEffects.Charged = true;
	StartDOT();
	ApplyChargedMaterial();
	GetWorldTimerManager().SetTimer(ClearChargedHandle, ClearChargedDelegate, StatusDuration, false);
}

void AEnemy::ClearStatusEffects()
{
	StatusEffects.Burning = false;
	StatusEffects.Wet = false;
	StatusEffects.Frost = false;
	StatusEffects.Charged = false;
	RemoveStatusMaterial();

	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AEnemy::OnHearNoise(APawn* OtherActor, const FVector& Location, float Volume)
{
	UE_LOG(LogTemp, Warning, TEXT("Heard actor!"));

	BlackboardComponent->SetValueAsBool(FName("bSensesPlayer"), true);
}

void AEnemy::GiveKillExp()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->AddExp(ExpOnKill);
}

void AEnemy::CalculateKillExp()
{
	ExpOnKill = Level * 10;
}

void AEnemy::ClearBurning()
{
	RemoveStatusMaterial();
	StatusEffects.Burning = false;
}

void AEnemy::ClearWet()
{
	RemoveStatusMaterial();
	StatusEffects.Wet = false;
}

void AEnemy::ClearFrost()
{
	RemoveStatusMaterial();
	StatusEffects.Frost = false;
}

void AEnemy::ClearCharged()
{
	RemoveStatusMaterial();
	StatusEffects.Charged = false;
}

void AEnemy::StartDOT()
{
	float DOTInterval = StatusEffects.Burning ? FireDOTInterval : ElectricDOTInterval;
	GetWorldTimerManager().SetTimer(ApplyDOTHandle, ApplyDOTDelegate, DOTInterval, true);
	DOTRemainingDuration = StatusDuration;
}

void AEnemy::ApplyDOT()
{
	if (DOTRemainingDuration <= 0 || (!StatusEffects.Burning && !StatusEffects.Charged))
	{
		GetWorldTimerManager().ClearTimer(ApplyDOTHandle);
		return;
	}
	
	float DOTInterval = StatusEffects.Burning ? FireDOTInterval : ElectricDOTInterval;
	int DamageOnDOT = StatusEffects.Burning ? FireDamageOnDOT : ElectricDamageOnDOT;
	DOTRemainingDuration -= DOTInterval;

	UGameplayStatics::ApplyDamage(
		this,
		DamageOnDOT,
		UGameplayStatics::GetPlayerController(this, 0),
		UGameplayStatics::GetPlayerCharacter(this, 0),
		UDamageType::StaticClass());
}

void AEnemy::Death()
{
	bIsAlive = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	AAIController* EnemyController = Cast<AAIController>(GetController());
	if (EnemyController)
	{
		EnemyController->StopMovement();
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("isDead"), true);
		// EnemyController->GetBrainComponent()->StopLogic(TEXT("Enemy died, stopping logic.\n"));
	}
	
	DetachFromControllerPendingDestroy();
	GetWorldTimerManager().SetTimer(DestroyHandle, DestroyDelegate, DeathDestroyDelay, false);
}

void AEnemy::DestroyAfterDeath()
{
	Destroy();
}

void AEnemy::RangeCheck()
{
	bool InRange = InChaseRange();
	BlackboardComponent->SetValueAsBool(FName("bInChaseRange"), InRange);

	if (!InRange)
	{
		BlackboardComponent->SetValueAsBool(FName("bSensesPlayer"), false);
	}
}

bool AEnemy::InChaseRange()
{
	float Dist = FVector::Distance(InitialLocation, GetActorLocation());
	return Dist < ChaseRange;
}
