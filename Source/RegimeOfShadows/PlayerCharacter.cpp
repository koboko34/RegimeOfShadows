// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InteractInterface.h"
#include "AbilityComponent.h"
#include "FireBasicProjectile.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	Camera = CreateDefaultSubobject<UCameraComponent>("Camera");
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->bUsePawnControlRotation = true;

	GetMesh()->SetOnlyOwnerSee(true);
	GetMesh()->SetupAttachment(Camera);
	GetMesh()->bCastDynamicShadow = false;
	GetMesh()->CastShadow = false;

	AbilityComponent = CreateDefaultSubobject<UAbilityComponent>("AbilityComponent");

	ActiveElement = Element::Fire;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	Experience = 0;
	ExpToNextLevel = pow(2, Level) * 10;

	StatsTickDelegate.BindUObject(this, &APlayerCharacter::StatsTick);
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
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &APlayerCharacter::Interact);
		EnhancedInputComponent->BindAction(OverchargeAction, ETriggerEvent::Started, this, &APlayerCharacter::Overcharge);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Triggered, this, &APlayerCharacter::BasicAttack);
		EnhancedInputComponent->BindAction(BasicAbilityAction, ETriggerEvent::Started, this, &APlayerCharacter::BasicAbility);
		EnhancedInputComponent->BindAction(BasicAbilityAction, ETriggerEvent::Completed, this, &APlayerCharacter::BasicAbilityEnd);
		EnhancedInputComponent->BindAction(StrongAbilityAction, ETriggerEvent::Started, this, &APlayerCharacter::StrongAbility);
		EnhancedInputComponent->BindAction(SwapToFireAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToFire);
		EnhancedInputComponent->BindAction(SwapToIceAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToIce);
		EnhancedInputComponent->BindAction(SwapToElectricAction, ETriggerEvent::Started, this, &APlayerCharacter::SwapToElectric);
	}
}

void APlayerCharacter::AddExp(int ExpToAdd)
{
	Experience += ExpToAdd;
	if (Experience > ExpToNextLevel)
		LevelUp();
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

void APlayerCharacter::Move(const FInputActionValue& Value)
{
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{ 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void APlayerCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookAxisVector = Value.Get<FVector2D>();

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
		GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
		return;
	}

	float AdjustedDrain = StaminaDrainPerSecond * GetWorld()->GetDeltaSeconds();
	Stamina -= AdjustedDrain;
	if (Stamina < 0)
		Stamina = 0;

	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
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
}

void APlayerCharacter::SwapToFire(const FInputActionValue& Value)
{
	ActiveElement = Element::Fire;
	UE_LOG(LogTemp, Warning, TEXT("Swapping to Fire\n"));
}

void APlayerCharacter::SwapToIce(const FInputActionValue& Value)
{
	ActiveElement = Element::Ice;
	UE_LOG(LogTemp, Warning, TEXT("Swapping to Ice\n"));
}

void APlayerCharacter::SwapToElectric(const FInputActionValue& Value)
{
	ActiveElement = Element::Electric;
	UE_LOG(LogTemp, Warning, TEXT("Swapping to Electric\n"));
}

void APlayerCharacter::Overcharge(const FInputActionValue& Value)
{
}

void APlayerCharacter::UseMana(int ManaToUse)
{
	Mana -= ManaToUse;
	UE_LOG(LogTemp, Warning, TEXT("%i mana left."), Mana);
}

void APlayerCharacter::LevelUp()
{
	Level++;
	ExpToNextLevel *= 2;
	if (Experience > ExpToNextLevel)
		LevelUp();
}

void APlayerCharacter::StatsTick()
{
	Stamina += 2;
}
