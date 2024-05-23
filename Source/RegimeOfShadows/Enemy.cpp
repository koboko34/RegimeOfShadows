// Armand Yilinkou, 2023


#include "Enemy.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


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

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;

	SetRootComponent(GetCapsuleComponent());
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));

	ClearBurningDelegate.BindUObject(this, &AEnemy::ClearBurning);
	ClearWetDelegate.BindUObject(this, &AEnemy::ClearWet);
	ClearFrostDelegate.BindUObject(this, &AEnemy::ClearFrost);
	ClearChargedDelegate.BindUObject(this, &AEnemy::ClearCharged);
	ApplyDOTDelegate.BindUObject(this, &AEnemy::ApplyDOT);
	DestroyDelegate.BindUObject(this, &AEnemy::DestroyAfterDeath);
	AliveSoundDelegate.BindUObject(this, &AEnemy::PlayAliveSound);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	ClearStatusEffects();

	AAIController* EnemyController = Cast<AAIController>(GetController());
	BlackboardComponent = EnemyController->GetBlackboardComponent();
	BlackboardComponent->SetValueAsBool(FName("bSensesPlayer"), false);
	BlackboardComponent->SetValueAsBool(FName("bUpdateBossStage"), false);
	BlackboardComponent->SetValueAsBool(FName("bIsAwake"), bIsAwake);
	InitialLocation = GetActorLocation();
	BlackboardComponent->SetValueAsVector(FName("InitialLocation"), InitialLocation);

	GetWorldTimerManager().SetTimer(RangeCheckHandle, RangeCheckDelegate, 1.f, true);
	GetWorldTimerManager().SetTimer(AliveSoundHandle, AliveSoundDelegate, FMath::RandRange(5.f, 10.f), false);
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bIsAlive || !bInChase)
		return DamageAmount;
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (bCanScream && !bHasScreamed && Health <= (int)((float)MaxHealth * BossStageHealthThreshold))
	{
		UE_LOG(LogTemp, Warning, TEXT("Updating boss stage!!"));
		BlackboardComponent->SetValueAsBool(FName("bUpdateBossStage"), true);
	}

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
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed * (1 - FrostSpeedDebuff);
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

	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
	GetWorldTimerManager().ClearAllTimersForObject(this);
}

void AEnemy::OnHearNoise(APawn* OtherActor, const FVector& Location, float Volume)
{
	BlackboardComponent->SetValueAsBool(FName("bSensesPlayer"), true);
}

void AEnemy::GiveKillExp()
{
	APlayerCharacter* Player = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	Player->AddExp(ExpOnKill);
}

void AEnemy::SetHasScreamed(bool NewHasScreamed)
{
	bHasScreamed = NewHasScreamed;
	BlackboardComponent->SetValueAsBool(FName("bUpdateBossStage"), false);
}

void AEnemy::SetInChase(bool NewInChase)
{
	bInChase = NewInChase;
}

void AEnemy::SetIsAwake(bool NewIsAwake)
{
	bIsAwake = NewIsAwake;
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
	GetCharacterMovement()->MaxWalkSpeed = BaseMovementSpeed;
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
	bInChase = false;
	GiveKillExp();

	APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>(UGameplayStatics::GetPlayerCharacter(this, 0));
	if (PlayerCharacter)
	{
		PlayerCharacter->RemoveFromBattleList(this);
	}

	AAIController* EnemyController = Cast<AAIController>(GetController());
	if (EnemyController)
	{
		EnemyController->StopMovement();
		EnemyController->GetBlackboardComponent()->SetValueAsBool(FName("isDead"), true);
	}
	
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1, ECollisionResponse::ECR_Ignore);

	GetWorldTimerManager().ClearTimer(AliveSoundHandle);
	if (DeathSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation(), DeathSoundVolume);
	}

	PawnSensingComponent->SetActive(false);
	PawnSensingComponent->bHearNoises = false;
	PawnSensingComponent->Deactivate();
	GetWorldTimerManager().SetTimer(DestroyHandle, DestroyDelegate, DeathDestroyDelay, false);
	UE_LOG(LogTemp, Warning, TEXT("Timer started!"));
	DetachFromControllerPendingDestroy();
}

void AEnemy::DestroyAfterDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("Destroying..."));
	Destroy();
}

void AEnemy::PlayAliveSound()
{
	if (AliveSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, AliveSound, GetActorLocation(), AliveSoundVolume);
	}

	GetWorldTimerManager().SetTimer(AliveSoundHandle, AliveSoundDelegate, FMath::RandRange(5.f, 10.f), false);
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
