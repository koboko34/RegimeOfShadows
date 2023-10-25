// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"

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

	ActiveElement = Element::Fire;

	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	Experience = 0;
	ExpToNextLevel = pow(2, Level) * 10;
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

		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
		EnhancedInputComponent->BindAction(OverchargeAction, ETriggerEvent::Started, this, &APlayerCharacter::Overcharge);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::BasicAttack);
		EnhancedInputComponent->BindAction(BasicAbilityAction, ETriggerEvent::Started, this, &APlayerCharacter::BasicAbility);
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
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
}

void APlayerCharacter::StopSprint(const FInputActionValue& Value)
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
}

void APlayerCharacter::Dodge(const FInputActionValue& Value)
{
}

void APlayerCharacter::Interact(const FInputActionValue& Value)
{
}

void APlayerCharacter::BasicAttack(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("BasicAttack()\n"));
}

void APlayerCharacter::BasicAbility(const FInputActionValue& Value)
{
}

void APlayerCharacter::StrongAbility(const FInputActionValue& Value)
{
}

void APlayerCharacter::SwapToFire(const FInputActionValue& Value)
{
	ActiveElement = Element::Fire;
}

void APlayerCharacter::SwapToIce(const FInputActionValue& Value)
{
	ActiveElement = Element::Ice;
}

void APlayerCharacter::SwapToElectric(const FInputActionValue& Value)
{
	ActiveElement = Element::Electric;
}

void APlayerCharacter::Overcharge(const FInputActionValue& Value)
{
}

void APlayerCharacter::LevelUp()
{
	Level++;
	ExpToNextLevel *= 2;
	if (Experience > ExpToNextLevel)
		LevelUp();
}
