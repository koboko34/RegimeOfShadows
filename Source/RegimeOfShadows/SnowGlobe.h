// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowGlobe.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API ASnowGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASnowGlobe();

private:
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Radius = 500;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float Lifetime = 10;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int ManaPerTick = 10;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	int DamagePerTick = 3;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float IceburnMultiplier = 2.f;

	float Interval = 1.f;
	float RemainingTime;

	FTimerHandle TickHandle;
	FTimerDelegate TickDelegate;

	AController* PlayerController;
	ACharacter* PlayerCharacter;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleTick();
};
