// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "BaseEntity.h"
#include "InputActionValue.h"
#include "LifeStealInterface.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
class UAbilityComponent;

UENUM(BlueprintType)
enum class Element : uint8
{
	None		UMETA(DisplayName = "None"),
	Fire		UMETA(DisplayName = "Fire"),
	Ice			UMETA(DisplayName = "Ice"),
	Electric	UMETA(DisplayName = "Electric")
};

UCLASS()
class REGIMEOFSHADOWS_API APlayerCharacter : public ABaseEntity, public ILifeStealInterface
{
	GENERATED_BODY()

public:
	APlayerCharacter();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	USceneComponent* MeleeAnchor;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeleeWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SprintAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InteractAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DodgeAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* PauseAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ToggleMapAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* OverchargeAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BasicAttackAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* BasicAbilityAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* StrongAbilityAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapToFireAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapToIceAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* SwapToElectricAction;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* CancelAbilityAction;

	float SpellSpeed = 1.f;

private:
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int Experience;
	UPROPERTY(VisibleAnywhere, Category = Stats, meta = (AllowPrivateAccess = true))
	int ExpToNextLevel;
	
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float MouseSensitivity = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	int WalkSpeed = 600;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	int SprintSpeed = 1000;
	bool bSprinting = false;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float SpeedUpMultiplier = 1.5f;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float SlowDownMultiplier = 0.8f;
	float MoveSpeedMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float SprintStaminaDrainPerSecond = 10.f;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	float DodgeVelocity = 1800.f;
	UPROPERTY(EditDefaultsOnly, Category = Movement, meta = (AllowPrivateAccess = true))
	int DodgeCost = 10;

	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float DodgeDuration = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	float LifeStealPerFlow = 0.01f;
	FTimerHandle DodgeHandle;
	FTimerDelegate DodgeDelegate;
	int Flow = 0;
	UPROPERTY(EditDefaultsOnly, Category = Combat, meta = (AllowPrivateAccess = true))
	int MaxFlow = 10;
	TArray<AActor*> DodgedActors;

	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float InteractDistance = 800.f;
	float CooldownFactor = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float SwapElementCooldown = 1.f;
	FTimerHandle SwapElementHandle;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float OverchargeCooldown = 60.f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float OverchargeDuration = 30.f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float OverchargeHealthCost = 0.25f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float DefaultSpellSpeed = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float OverchargeSpellSpeed = 2.f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float DefaultCooldownFactor = 1.f;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	float OverchargeCooldownFactor = 2.f;

	Element OverchargedElement;
	FTimerHandle OverchargeDurationHandle;
	FTimerHandle OverchargeCooldownHandle;
	FTimerDelegate OverchargeDelegate;
	FTimerHandle OverchargeDOTHandle;
	FTimerDelegate OverchargeDOTDelegate;
	float RemainingTimeDOT;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	int ManaPerTick = 5;
	UPROPERTY(EditDefaultsOnly, Category = Other, meta = (AllowPrivateAccess = true))
	int StaminaPerTick = 5;
	bool bRecoverMana = false;
	
	bool bStrongAbilityDown = false;

	Element ActiveElement;
	UPROPERTY(EditAnywhere, Category = Abilities)
	UAbilityComponent* AbilityComponent;

	class UPawnNoiseEmitterComponent* PawnNoiseEmitterComponent;

	FTimerHandle NoiseHandle;
	FTimerDelegate NoiseDelegate;

	FTimerHandle StatsTickHandle;
	FTimerDelegate StatsTickDelegate;

	ABaseEntity* CurrentBoss = nullptr;
	ABaseEntity* CurrentBossTwo = nullptr;

	UPROPERTY()
	FString SettingsSaveSlot = "SettingsSaveSlot";
	UPROPERTY()
	FString PlayerStatsSaveSlot = "PlayerStatsSaveSlot";

	UPROPERTY(EditDefaultsOnly, Category = Sound, meta = (AllowPrivateAccess = true))
	USoundMix* MasterSoundMix;
	UPROPERTY(EditDefaultsOnly, Category = Sound, meta = (AllowPrivateAccess = true))
	USoundClass* MasterSoundClass;
	UPROPERTY(EditDefaultsOnly, Category = Sound, meta = (AllowPrivateAccess = true))
	USoundClass* MusicSoundClass;

	TArray<AActor*> BattleList;
	bool bInBattle = false;

public:
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void ApplyLifeSteal(float Damage) override;

	UCameraComponent* GetCameraComponent() const { return Camera; }

	UFUNCTION(BlueprintCallable)
	Element GetActiveElement() const { return ActiveElement; }

protected:
	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

public:
	void AddExp(int ExpToAdd);
	void AddHealth(int HealthToAdd);
	void AddMana(int ManaToAdd);
	void UseMana(int ManaToUse);

	void ResetFireQProj();

	UFUNCTION(BlueprintPure)
	UAbilityComponent* GetAbilityComponent() const { return AbilityComponent; }
	
	UFUNCTION(BlueprintPure)
	float GetMouseSensitivity() const { return MouseSensitivity; }
	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float NewSens);

	UFUNCTION(BlueprintPure)
	int GetFlow() const { return Flow; }
	UFUNCTION(BlueprintPure)
	bool IsOvercharged() const { return GetWorldTimerManager().IsTimerActive(OverchargeDurationHandle); }
	UFUNCTION(BlueprintPure)
	Element GetOverchargedElement() const { return OverchargedElement; }
	UFUNCTION(BlueprintPure)
	float GetOverchargeRemainingTime() const { return GetWorldTimerManager().GetTimerRemaining(OverchargeDurationHandle); }
	UFUNCTION(BlueprintPure)
	float GetOverchargeCooldown() const { return GetWorldTimerManager().GetTimerRemaining(OverchargeCooldownHandle); }

	bool GetStrongAbilityDown() const { return bStrongAbilityDown; }
	float GetSpellSpeed() const { return SpellSpeed; }
	float GetCooldownFactor() const { return CooldownFactor; }

	UFUNCTION(BlueprintPure)
	int GetExperience() const { return Experience; }
	UFUNCTION(BlueprintPure)
	int GetExperienceToNextLevel() const { return ExpToNextLevel; }
	UFUNCTION(BlueprintPure)
	int GetPlayerLevel() const { return Level; }

	UFUNCTION(BlueprintImplementableEvent)
	void HandlePause();

	UFUNCTION(BlueprintPure)
	ABaseEntity* GetCurrentBoss() const { return CurrentBoss; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentBoss(ABaseEntity* NewBoss);
	UFUNCTION(BlueprintPure)
	ABaseEntity* GetCurrentBossTwo() const { return CurrentBossTwo; }
	UFUNCTION(BlueprintCallable)
	void SetCurrentBossTwo(ABaseEntity* NewBoss);

	UFUNCTION(BlueprintCallable)
	void SaveSettings();
	UFUNCTION(BlueprintCallable)
	bool LoadSettings();
	UFUNCTION(BlueprintCallable)
	void SavePlayerStats();
	UFUNCTION(BlueprintCallable)
	bool LoadPlayerStats();

	UFUNCTION(BlueprintCallable)
	void AddToBattleList(AActor* Actor);
	UFUNCTION(BlueprintCallable)
	void RemoveFromBattleList(AActor* Actor);

	UFUNCTION(BlueprintImplementableEvent)
	void PlayCalmMusic();
	UFUNCTION(BlueprintImplementableEvent)
	void PlayBattleMusic();

	UFUNCTION(BlueprintImplementableEvent)
	void WeaponSwing();

private:
	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void Sprint(const FInputActionValue& Value);
	void StopSprint(const FInputActionValue& Value);
	void Dodge(const FInputActionValue& Value);
	void Interact(const FInputActionValue& Value);
	void ToggleMap(const FInputActionValue& Value);
	void Pause(const FInputActionValue& Value);

	void BasicAttack(const FInputActionValue& Value);
	void BasicAbility(const FInputActionValue& Value);
	void BasicAbilityEnd(const FInputActionValue& Value);
	void StrongAbility(const FInputActionValue& Value);
	void StrongAbilityEnd(const FInputActionValue& Value);

	void SwapToFire(const FInputActionValue& Value);
	void SwapToIce(const FInputActionValue& Value);
	void SwapToElectric(const FInputActionValue& Value);

	void Overcharge(const FInputActionValue& Value);
	void CancelAbility(const FInputActionValue& Value);

	void LevelUp();

	UFUNCTION()
	void StatsTick();

	UFUNCTION()
	void DodgeEnd();

	void FireOvercharge();
	void IceOvercharge();
	void ElectricOvercharge();
	UFUNCTION()
	void OverchargeEnd();
	UFUNCTION()
	void FireOverchargeDOT();

	UFUNCTION()
	void MakePawnNoise();

	void PlayerDeath();

};
