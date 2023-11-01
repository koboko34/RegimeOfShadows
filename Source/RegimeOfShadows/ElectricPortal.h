// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ElectricPortal.generated.h"

class UAbilityComponent;

UCLASS()
class REGIMEOFSHADOWS_API AElectricPortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AElectricPortal();

	UPROPERTY(EditAnywhere)
	UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	// virtual void Tick(float DeltaTime) override;

private:
	AElectricPortal* PartnerPortal;

	FTimerHandle PulseHandle;
	FTimerDelegate PulseDelegate;
	float RemainingTime;

	AActor* PlayerActor;

	UAbilityComponent* AbilityComponent;

public:
	void Init(AElectricPortal* Portal, UAbilityComponent* InAbilityComponent);
	AElectricPortal* GetPartnerPortal() const { return PartnerPortal; }

	UPROPERTY(EditAnywhere)
	float SweepZOffset = 50.f;

private:
	UFUNCTION()
	void Pulse();

	void Sweep();
};
