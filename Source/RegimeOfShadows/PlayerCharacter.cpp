// Armand Yilinkou, 2023


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractInterface.h"
#include "AbilityComponent.h"
#include "FireBasicProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "SettingsSaveGame.h"
#include "PlayerStatsSaveGame.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->bUsePawnControlRotation = true;

	MeleeAnchor = CreateDefaultSubobject<USceneComponent>("MeleeAnchor");
	MeleeAnchor->SetupAttachment(Camera);

	MeleeWeapon = CreateDefaultSubobject<UStaticMeshComponent>("MeleeWeapon");
	MeleeWeapon->SetupAttachment(MeleeAnchor);
	MeleeWeapon->CastShadow = false;
	MeleeWeapon->bReceivesDecals = false;
	MeleeWeapon->SetOnlyOwnerSee(true);
	MeleeWeapon->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	MeleeWeapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	GetMesh()->SetOnlyOwnerSee(true);
	GetMesh()->SetupAttachment(Camera);
	GetMesh()->bCastDynamicShadow = false;
	GetMesh()->CastShadow = false;
	GetMesh()->bReceivesDecals = false;

	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>("AbilityComponent");

	ActiveElement = Element::Fire;
	OverchargedElement = Element::None;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * MoveSpeedMultiplier;
	SpellSpeed = DefaultSpellSpeed;
	CooldownFactor = DefaultCooldownFactor;

	Experience = 0;
	ExpToNextLevel = pow(2, Level) * 10;

	PawnNoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>("PawnNoiseEmitterComponent");

	StatsTickDelegate.BindUObject(this, &APlayerCharacter::StatsTick);
	OverchargeDelegate.BindUObject(this, &APlayerCharacter::OverchargeEnd);
	OverchargeDOTDelegate.BindUObject(this, &APlayerCharacter::FireOverchargeDOT);
	DodgeDelegate.BindUObject(this, &APlayerCharacter::DodgeEnd);
	NoiseDelegate.BindUObject(this, &APlayerCharacter::MakePawnNoise);
}

float APlayerCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if (!bIsAlive)
	{
		return DamageAmount;
	}
	
	if (GetWorldTimerManager().IsTimerActive(DodgeHandle) && DamageCauser != this)
	{
		if (DodgedActors.Contains(DamageCauser))
			return DamageAmount;
		
		Flow = std::min(MaxFlow, Flow + 1);
		DodgedActors.Add(DamageCauser);
		return DamageAmount;
	}
	
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);

	if (DamageCauser != this)
		Flow = std::max(0, Flow - 5);

	UE_LOG(LogTemp, Warning, TEXT("Player has %i health remaining"), Health);

	if (Health <= 0)
	{
		PlayerDeath();
	}

	return DamageAmount;
}

void APlayerCharacter::ApplyLifeSteal(float Damage)
{
	if (Flow <= 0)
		return;

	AddHealth(Damage * (LifeStealPerFlow * Flow));
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	GetWorldTimerManager().SetTimer(StatsTickHandle, StatsTickDelegate, 0.5f, true);
	GetWorldTimerManager().SetTimer(NoiseHandle, NoiseDelegate, 0.5f, true);

	if (!LoadSettings())
	{
		SaveSettings();
	}
	if (!LoadPlayerStats())
	{
		UE_LOG(LogTemp, Warning, TEXT("No PlayerStats found! Creating new save!"));
		SavePlayerStats();
	}

	Mana = MaxMana;

	EnableInput(UGameplayStatics::GetPlayerController(this, 0));
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Move);
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Look);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &APlayerCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &APlayerCharacter::StopSprint);

		EnhancedInputComponent->BindAction(ToggleMapAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleMap);
		EnhancedInputComponent->BindAction(PauseAction, ETriggerEvent::Started, this, &APlayerCharacter::Pause);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
		EnhancedInputComponent->BindAction(OverchargeAction, ETriggerEvent::Started, this, &APlayerCharacter::Overcharge);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicAttack);
		EnhancedInputComponent->BindAction(BasicAbilityAction, ETriggerEvent::Started, this, &APlayerCharacter::BasicAbility);
		EnhancedInputComponent->BindAction(BasicAbilityAction, ETriggerEvent::Completed, this, &APlayerCharacter::BasicAbilityEnd);
		EnhancedInputComponent->BindAction(StrongAbilityAction, ETriggerEvent::Triggered, this, &APlayerCharacter::StrongAbility);
		EnhancedInputComponent->BindAction(StrongAbilityAction, ETriggerEvent::Completed, this, &APlayerCharacter::StrongAbilityEnd);
		EnhancedInputComponent->BindAction(SwapToFireAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToFire);
		EnhancedInputComponent->BindAction(SwapToIceAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToIce);
		EnhancedInputComponent->BindAction(SwapToElectricAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToElectric);
		EnhancedInputComponent->BindAction(CancelAbilityAction, ETriggerEvent::Started, this, &APlayerCharacter::CancelAbility);
	}
}

void APlayerCharacter::AddExp(int ExpToAdd)
{
	UE_LOG(LogTemp, Warning, TEXT("Adding exp..."));
	
	Experience += ExpToAdd;
	if (Experience >= ExpToNextLevel)
		LevelUp();
}

void APlayerCharacter::AddHealth(int HealthToAdd)
{
	Health = std::min(MaxHealth, Health + HealthToAdd);

	UE_LOG(LogTemp, Warning, TEXT("Health: %i"), Health);
}

void APlayerCharacter::AddMana(int ManaToAdd)
{
	Mana = std::min(MaxMana, Mana + ManaToAdd);

	UE_LOG(LogTemp, Warning, TEXT("Mana: %i"), Mana);
}

void APlayerCharacter::ResetFireQProj()
{
	if (AbilityComponent->ActiveFireQProj)
		AbilityComponent->ActiveFireQProj = nullptr;
}

void APlayerCharacter::SetMouseSensitivity(float NewSens)
{
	MouseSensitivity = NewSens;
}

void APlayerCharacter::SetCurrentBoss(ABaseEntity* NewBoss)
{
	CurrentBoss = NewBoss;
}

void APlayerCharacter::SetCurrentBossTwo(ABaseEntity* NewBoss)
{
	CurrentBossTwo = NewBoss;
}

void APlayerCharacter::SaveSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SettingsSaveSlot, 0))
	{
		USettingsSaveGame* SettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSaveSlot, 0));
		SettingsSaveGame->SetMouseSensitivity(MouseSensitivity);
		UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SettingsSaveSlot, 0);
	}
	else
	{
		USettingsSaveGame* SettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::CreateSaveGameObject(USettingsSaveGame::StaticClass()));
		SettingsSaveGame->SetMouseSensitivity(MouseSensitivity);
		UGameplayStatics::SaveGameToSlot(SettingsSaveGame, SettingsSaveSlot, 0);
	}
	UE_LOG(LogTemp, Warning, TEXT("Settings saved!"));
}

bool APlayerCharacter::LoadSettings()
{
	if (UGameplayStatics::DoesSaveGameExist(SettingsSaveSlot, 0))
	{
		USettingsSaveGame* SettingsSaveGame = Cast<USettingsSaveGame>(UGameplayStatics::LoadGameFromSlot(SettingsSaveSlot, 0));
		MouseSensitivity = SettingsSaveGame->GetMouseSensitivity();
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MasterSoundClass, SettingsSaveGame->GetMasterVolume(), 1.f, 0.f);
		UGameplayStatics::SetSoundMixClassOverride(this, MasterSoundMix, MusicSoundClass, SettingsSaveGame->GetMusicVolume(), 1.f, 0.f);
		UE_LOG(LogTemp, Warning, TEXT("Settings loaded!"));
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("Settings save does not exist!"));
	return false;
}

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{ 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}

	MakeNoise(1.f, this, GetActorLocation(), 1000.f, FName("WalkingNoise"));
	AbilityComponent->CancelAbilityCast();
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();
	LookAxisVector *= MouseSensitivity;

	if (Controller != nullptr)
	{
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void APlayerCharacter::Sprint(const FInputActionValue& Value)
{
	if (Stamina < 10)
	{
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * MoveSpeedMultiplier;
		bSprinting = false;
		return;
	}

	if (GetVelocity().Length() > 1)
	{
		float AdjustedDrain = SprintStaminaDrainPerSecond * GetWorld()->GetDeltaSeconds();
		Stamina -= AdjustedDrain;
		if (Stamina < 0)
			Stamina = 0;
	}

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed * MoveSpeedMultiplier;
	bSprinting = true;
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed * MoveSpeedMultiplier;
	bSprinting = false;
}

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
	if (!GetCharacterMovement()->IsMovingOnGround() || Stamina < DodgeCost)
		return;

	Stamina -= DodgeCost;
	FVector LaunchVector;
	if (GetVelocity().Length() > 0.1)
	{
		FVector NormalizedVelocity = GetVelocity();
		NormalizedVelocity.Normalize();
		LaunchVector = FVector(NormalizedVelocity.X * DodgeVelocity, NormalizedVelocity.Y * DodgeVelocity, 100);
	}
	else
	{
		FVector NormalizedVector = GetActorForwardVector();
		NormalizedVector.Normalize();
		LaunchVector = FVector(NormalizedVector.X * DodgeVelocity, NormalizedVector.Y * DodgeVelocity, 100);
	}
	
	GetCharacterMovement()->StopMovementImmediately();
	LaunchCharacter(LaunchVector, false, false);
	GetWorldTimerManager().SetTimer(DodgeHandle, DodgeDelegate, DodgeDuration, false);
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Interact()"));
	
	FVector Start = Camera->GetComponentLocation();
	FVector End = Start + Camera->GetForwardVector() * InteractDistance;
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	// ECC_GameTraceChannel2 == Interact trace channel
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECollisionChannel::ECC_GameTraceChannel2, Params))
	{
		if (HitResult.GetActor()->Implements<UInteractInterface>())
		{
			UE_LOG(LogTemp, Warning, TEXT("Interacting with %s"), *HitResult.GetActor()->GetName());
			IInteractInterface::Execute_Interact(HitResult.GetActor());
		}
	}

	DrawDebugLine(GetWorld(), Start, End, FColor::Emerald, false, 5);
}

void APlayerCharacter::ToggleMap(const FInputActionValue& Value)
{
}

void APlayerCharacter::Pause(const FInputActionValue& Value)
{
	HandlePause();
}

void APlayerCharacter::BasicAttack(const FInputActionValue& Value)
{
	switch (ActiveElement)
	{
	case Element::Fire:
		AbilityComponent->BasicAttackFire();
		break;
	case Element::Ice:
		AbilityComponent->BasicAttackIce();
		break;
	case Element::Electric:
		AbilityComponent->BasicAttackElectric();
		break;
	default:
		break;
	}
}

void APlayerCharacter::BasicAbility(const FInputActionValue& Value)
{
	if (AbilityComponent->ActiveFireQProj)
	{
		AbilityComponent->ActiveFireQProj->Explode();
		return;
	}
	
	switch (ActiveElement)
	{
	case Element::Fire:
		AbilityComponent->BasicAbilityFire();
		break;
	case Element::Ice:
		AbilityComponent->BasicAbilityIce();
		break;
	case Element::Electric:
		AbilityComponent->BasicAbilityElectric();
		break;
	default:
		break;
	}
}

void APlayerCharacter::BasicAbilityEnd(const FInputActionValue& Value)
{
	switch (ActiveElement)
	{
	case Element::Fire:
		AbilityComponent->BasicAbilityFireEnd();
		break;
	default:
		break;
	}
}


void APlayerCharacter::StrongAbility(const FInputActionValue& Value)
{
	switch (ActiveElement)
	{
	case Element::Fire:
		AbilityComponent->StrongAbilityFire();
		break;
	case Element::Ice:
		AbilityComponent->StrongAbilityIce();
		break;
	case Element::Electric:
		AbilityComponent->StrongAbilityElectric();
		break;
	default:
		break;
	}
	
	bStrongAbilityDown = true;
}

void APlayerCharacter::StrongAbilityEnd(const FInputActionValue& Value)
{
	bStrongAbilityDown = false;

	switch (ActiveElement)
	{
	case Element::Fire:
		AbilityComponent->StrongAbilityFireEnd();
		break;
	default:
		break;
	}
}

void APlayerCharacter::SwapToFire(const FInputActionValue& Value)
{
	if (GetWorldTimerManager().IsTimerActive(SwapElementHandle) || ActiveElement == Element::Fire)
		return;

	ActiveElement = Element::Fire;
	GetWorldTimerManager().SetTimer(SwapElementHandle, SwapElementCooldown, false);
	UE_LOG(LogTemp, Warning, TEXT("Swapping to Fire\n"));
}

void APlayerCharacter::SwapToIce(const FInputActionValue& Value)
{
	if (GetWorldTimerManager().IsTimerActive(SwapElementHandle) || ActiveElement == Element::Ice)
		return;
	
	ActiveElement = Element::Ice;
	GetWorldTimerManager().SetTimer(SwapElementHandle, SwapElementCooldown, false);
	UE_LOG(LogTemp, Warning, TEXT("Swapping to Ice\n"));
}

void APlayerCharacter::SwapToElectric(const FInputActionValue& Value)
{
	if (GetWorldTimerManager().IsTimerActive(SwapElementHandle) || ActiveElement == Element::Electric)
		return;
	
	ActiveElement = Element::Electric;
	GetWorldTimerManager().SetTimer(SwapElementHandle, SwapElementCooldown, false);
	UE_LOG(LogTemp, Warning, TEXT("Swapping to Electric\n"));
}

void APlayerCharacter::Overcharge(const FInputActionValue& Value)
{
	if (GetWorldTimerManager().IsTimerActive(OverchargeDurationHandle) || GetWorldTimerManager().IsTimerActive(OverchargeCooldownHandle))
		return;
	
	switch (ActiveElement)
	{
	case Element::Fire:
		FireOvercharge();
		break;
	case Element::Ice:
		IceOvercharge();
		break;
	case Element::Electric:
		ElectricOvercharge();
		break;
	default:
		break;
	}

	GetWorldTimerManager().SetTimer(OverchargeDurationHandle, OverchargeDelegate, OverchargeDuration, false);
}

void APlayerCharacter::CancelAbility(const FInputActionValue& Value)
{
	AbilityComponent->CancelAbility();
}

void APlayerCharacter::UseMana(int ManaToUse)
{
	Mana -= ManaToUse;
	UE_LOG(LogTemp, Warning, TEXT("%i mana left."), Mana);
}

void APlayerCharacter::LevelUp()
{
	Level++;
	UE_LOG(LogTemp, Warning, TEXT("Advanced to Level %i"), Level);
	
	MaxHealth += 10;
	Health = MaxHealth;

	Experience -= ExpToNextLevel;
	ExpToNextLevel *= 2;

	if (Experience > ExpToNextLevel)
		LevelUp();
}

void APlayerCharacter::StatsTick()
{
	if (!bSprinting)
		Stamina = std::min(MaxStamina, (int)Stamina + StaminaPerTick);
	
	if (bRecoverMana)
		Mana = std::min(MaxMana, Mana + ManaPerTick);
}

void APlayerCharacter::DodgeEnd()
{
	DodgedActors.Empty();
}

void APlayerCharacter::FireOvercharge()
{
	GetWorldTimerManager().SetTimer(OverchargeDOTHandle, OverchargeDOTDelegate, 1, true);
	RemainingTimeDOT = OverchargeDuration;
	CooldownFactor = OverchargeCooldownFactor;
	OverchargedElement = Element::Fire;
}

void APlayerCharacter::IceOvercharge()
{
	MoveSpeedMultiplier = SlowDownMultiplier;
	GetCharacterMovement()->MaxWalkSpeed *= MoveSpeedMultiplier;
	bRecoverMana = true;
	OverchargedElement = Element::Ice;
}

void APlayerCharacter::ElectricOvercharge()
{
	int Dmg;
	if (Health < MaxHealth * OverchargeHealthCost)
	{
		Dmg = Health - MaxHealth * 0.01;
	}
	else
	{
		Dmg = MaxHealth * OverchargeHealthCost;
	}

	UGameplayStatics::ApplyDamage(this, Dmg, GetController(), this, UDamageType::StaticClass());
	StaminaPerTick *= 2;
	MoveSpeedMultiplier = SpeedUpMultiplier;
	GetCharacterMovement()->MaxWalkSpeed *= MoveSpeedMultiplier;
	SpellSpeed = OverchargeSpellSpeed;
	OverchargedElement = Element::Electric;
}

void APlayerCharacter::OverchargeEnd()
{
	GetWorldTimerManager().SetTimer(OverchargeCooldownHandle, OverchargeCooldown - OverchargeDuration, false);

	bRecoverMana = false;
	StaminaPerTick /= 2;
	MoveSpeedMultiplier = 1.f;
	GetCharacterMovement()->MaxWalkSpeed /= MoveSpeedMultiplier;
	SpellSpeed = DefaultSpellSpeed;
	CooldownFactor = DefaultCooldownFactor;
	OverchargedElement = Element::None;

	UE_LOG(LogTemp, Warning, TEXT("Overcharge finished!"));
}

void APlayerCharacter::FireOverchargeDOT()
{
	if (Health > MaxHealth * 0.01)
		UGameplayStatics::ApplyDamage(this, MaxHealth * 0.01, GetController(), this, UDamageType::StaticClass());

	RemainingTimeDOT -= 1;
	if (RemainingTimeDOT <= 0)
		GetWorldTimerManager().ClearTimer(OverchargeDOTHandle);
}

void APlayerCharacter::MakePawnNoise()
{
	PawnNoiseEmitterComponent->MakeNoise(this, 1.f, GetActorLocation());
}

void APlayerCharacter::PlayerDeath()
{
	UE_LOG(LogTemp, Warning, TEXT("PlayerDeath() trigerred."));
	
	bIsAlive = false;
	DisableInput(UGameplayStatics::GetPlayerController(this, 0));
}

bool APlayerCharacter::LoadPlayerStats()
{
	if (UGameplayStatics::DoesSaveGameExist(PlayerStatsSaveSlot, 0))
	{
		UPlayerStatsSaveGame* PlayerStatsSaveGame = Cast<UPlayerStatsSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerStatsSaveSlot, 0));
		Level = PlayerStatsSaveGame->GetLevel();
		Experience = PlayerStatsSaveGame->GetExperience();
		ExpToNextLevel = PlayerStatsSaveGame->GetExpToNextLevel();
		UE_LOG(LogTemp, Warning, TEXT("PlayerStats loaded!"));
		return true;
	}
	UE_LOG(LogTemp, Warning, TEXT("PlayerStats save does not exist!"));
	return false;
}

void APlayerCharacter::AddToBattleList(AActor* Actor)
{
	if (BattleList.Contains(Actor))
	{
		return;
	}
	
	bool Empty = BattleList.IsEmpty();
	BattleList.Add(Actor);

	if (Empty && !bInBattle)
	{
		bInBattle = true;
		PlayBattleMusic();
	}
}

void APlayerCharacter::RemoveFromBattleList(AActor* Actor)
{
	BattleList.Remove(Actor);

	if (BattleList.IsEmpty() && bInBattle)
	{
		bInBattle = false;
		PlayCalmMusic();
	}
}

void APlayerCharacter::SavePlayerStats()
{
	if (UGameplayStatics::DoesSaveGameExist(PlayerStatsSaveSlot, 0))
	{
		UPlayerStatsSaveGame* PlayerStatsSaveGame = Cast<UPlayerStatsSaveGame>(UGameplayStatics::LoadGameFromSlot(PlayerStatsSaveSlot, 0));
		PlayerStatsSaveGame->SetLevel(Level);
		PlayerStatsSaveGame->SetExperience(Experience);
		PlayerStatsSaveGame->SetExpToNextLevel(ExpToNextLevel);
		UGameplayStatics::SaveGameToSlot(PlayerStatsSaveGame, PlayerStatsSaveSlot, 0);
	}
	else
	{
		UPlayerStatsSaveGame* PlayerStatsSaveGame = Cast<UPlayerStatsSaveGame>(UGameplayStatics::CreateSaveGameObject(UPlayerStatsSaveGame::StaticClass()));
		PlayerStatsSaveGame->SetLevel(Level);
		PlayerStatsSaveGame->SetExperience(Experience);
		PlayerStatsSaveGame->SetExpToNextLevel(ExpToNextLevel);
		UGameplayStatics::SaveGameToSlot(PlayerStatsSaveGame, PlayerStatsSaveSlot, 0);
	}
	UE_LOG(LogTemp, Warning, TEXT("PlayerStats saved!"));
}
