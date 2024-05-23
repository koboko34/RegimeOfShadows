// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricPortal.generated.h"

class UAbilityComponent;
class UNiagaraSystem;

UCLASS()
class REGIMEOFSHADOWS_API AElectricPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	AElectricPortal();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere)
	USceneComponent* ZapTarget;

protected:
	virtual void BeginPlay() override;

private:
	AElectricPortal* PartnerPortal;

	FTimerHandle PulseHandle;
	FTimerDelegate PulseDelegate;
	float RemainingTime;

	AActor* PlayerActor;
	AController* PlayerController;

	UAbilityComponent* AbilityComponent;

public:
	void Init(AElectricPortal* Portal, UAbilityComponent* InAbilityComponent);
	AElectricPortal* GetPartnerPortal() const { return PartnerPortal; }
	FVector GetZapTargetLocation() const { return ZapTarget->GetComponentLocation(); }

	UPROPERTY(EditAnywhere)
	float SweepZOffset = 50.f;
	UPROPERTY(EditDefaultsOnly)
	UNiagaraSystem* ElectricBeamSystem;

	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	USoundBase* LaserZapSound;
	UPROPERTY(EditDefaultsOnly, Category = Sounds)
	float LaserZapVolume = 1.f;

private:
	UFUNCTION()
	void Pulse();

	void Sweep();
};
