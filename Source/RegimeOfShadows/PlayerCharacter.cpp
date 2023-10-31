// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "Projectile.h"
#include "SnowGlobe.h"
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

		EnhancedInputComponent->BindAction(ToggleMapAction, ETriggerEvent::Started, this, &APlayerCharacter::ToggleMap);
		EnhancedInputComponent->BindAction(DodgeAction, ETriggerEvent::Started, this, &APlayerCharacter::Dodge);
		EnhancedInputComponent->BindAction(OverchargeAction, ETriggerEvent::Started, this, &APlayerCharacter::Overcharge);
		EnhancedInputComponent->BindAction(BasicAttackAction, ETriggerEvent::Started, this, &APlayerCharacter::BasicAttack);
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
	if (ActiveFireQProj)
		ActiveFireQProj = nullptr;
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

void APlayerCharacter::ToggleMap(const FInputActionValue& Value)
{
}

void APlayerCharacter::BasicAttack(const FInputActionValue& Value)
{
	switch (ActiveElement)
	{
	case Element::Fire:
		BasicAttackFire();
		break;
	case Element::Ice:
		BasicAttackIce();
		break;
	case Element::Electric:
		BasicAttackElectric();
		break;
	default:
		break;
	}
}

void APlayerCharacter::BasicAbility(const FInputActionValue& Value)
{
	switch (ActiveElement)
	{
	case Element::Fire:
		BasicAbilityFire();
		break;
	case Element::Ice:
		BasicAbilityIce();
		break;
	case Element::Electric:
		BasicAbilityElectric();
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
		BasicAbilityFireEnd();
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
		StrongAbilityFire();
		break;
	case Element::Ice:
		StrongAbilityIce();
		break;
	case Element::Electric:
		StrongAbilityElectric();
		break;
	default:
		break;
	}
}

void APlayerCharacter::BasicAttackFire()
{
	SpawnProjectile(FireAttackProj);
}

void APlayerCharacter::BasicAbilityFire()
{
	if (ActiveFireQProj && ActiveFireQProj->bActive)
	{
		ActiveFireQProj->Explode();
		return;
	}
	
	if (Mana < FireQManaCost)
		return;

	UseMana(FireQManaCost);

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	Params.Owner = this;
	ActiveFireQProj = GetWorld()->SpawnActor<AFireBasicProjectile>(
		FireQProj,
		Camera->GetComponentLocation() + Camera->GetForwardVector() * 200,
		Camera->GetComponentRotation(),
		Params
	);

	ActiveFireQProj->AttachToActor(this, FAttachmentTransformRules::KeepWorldTransform);
}

void APlayerCharacter::BasicAbilityFireEnd()
{
	if (ActiveFireQProj)
	{
		ActiveFireQProj->Release();
	}
}

void APlayerCharacter::StrongAbilityFire()
{
	if (Mana < FireEManaCost)
		return;

	UseMana(FireEManaCost);
}

void APlayerCharacter::BasicAttackIce()
{
	SpawnProjectile(IceAttackProj);
}

void APlayerCharacter::BasicAbilityIce()
{
	if (Mana < WaterAbilityManaCost)
		return;

	UseMana(WaterAbilityManaCost);
	SpawnProjectile(WaterAbilityProj);
}

void APlayerCharacter::StrongAbilityIce()
{
	if (Mana < SnowGlobeManaCost)
		return;

	UseMana(SnowGlobeManaCost);
	
	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FHitResult HitResult;
	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.AddIgnoredActor(this);
	GetWorld()->LineTraceSingleByChannel(
		HitResult,
		GetActorLocation(),
		GetActorLocation() + (GetActorUpVector() * 200 * -1),
		ECollisionChannel::ECC_WorldStatic,
		CollisionQueryParams
	);

	DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + (GetActorUpVector() * 200 * -1), FColor::Red, false, 5.f);
	
	FVector SpawnLocation = GetActorLocation();
	if (HitResult.bBlockingHit)
	{
		SpawnLocation = HitResult.ImpactPoint;
	}

	GetWorld()->SpawnActor<ASnowGlobe>(SnowGlobeClass, SpawnLocation, FRotator::ZeroRotator, Params);
}

void APlayerCharacter::BasicAttackElectric()
{
}

void APlayerCharacter::BasicAbilityElectric()
{
	if (Mana < ElectricBasicManaCost)
		return;

	UseMana(ElectricBasicManaCost);
	
	FHitResult OutHit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	FVector StartLocation = Camera->GetComponentLocation();
	GetWorld()->LineTraceSingleByChannel(OutHit, StartLocation, StartLocation + Camera->GetForwardVector() * 20000, ECollisionChannel::ECC_Visibility, Params);
	DrawDebugLine(GetWorld(), StartLocation, OutHit.bBlockingHit ? OutHit.ImpactPoint : (StartLocation + Camera->GetForwardVector() * 20000), FColor::Purple, false, 5.f);

	if (OutHit.bBlockingHit)
	{
		ElectricChainStart(OutHit.GetActor());
	}
}

void APlayerCharacter::StrongAbilityElectric()
{
	if (Mana < ElectricStrongManaCost)
		return;

	UseMana(ElectricStrongManaCost);
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

void APlayerCharacter::ElectricChainStart(AActor* HitActor)
{
	TArray<AActor*> HitActors;
	
	ElectricChainRecursive(HitActor, HitActors);
}

void APlayerCharacter::ElectricChainRecursive(AActor* HitActor, TArray<AActor*>& HitActors)
{	
	HitActors.Add(HitActor);

	// apply effects to this HitActor
	AEnemy* Enemy = Cast<AEnemy>(HitActor);
	if (!Enemy)
		return;

	if (Enemy->StatusEffects.Burning)
	{
		UGameplayStatics::ApplyDamage(HitActor, ElectricChainDamage * ElectricChainOverloadedMultiplier, GetController(), this, UDamageType::StaticClass());
		Enemy->ClearStatusEffects();
		return;
	}
	
	UGameplayStatics::ApplyDamage(HitActor, ElectricChainDamage, GetController(), this, UDamageType::StaticClass());

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
	QueryParams.AddIgnoredActor(this);
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

AProjectile* APlayerCharacter::SpawnProjectile(TSubclassOf<AProjectile> ProjectileToSpawn)
{
	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	AProjectile* Proj = GetWorld()->SpawnActor<AProjectile>(
		ProjectileToSpawn,
		Camera->GetComponentLocation() + Camera->GetForwardVector() * 200,
		Camera->GetComponentRotation(),
		Params);

	return Proj;
}
