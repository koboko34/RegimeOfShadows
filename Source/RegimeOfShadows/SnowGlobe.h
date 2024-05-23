// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SnowGlobe.generated.h"

UCLASS()
class REGIMEOFSHADOWS_API ASnowGlobe : public AActor
{
	GENERATED_BODY()
	
public:	
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

	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	USoundBase* WindSound;
	UPROPERTY(EditDefaultsOnly, meta = (AllowPrivateAccess = true))
	float WindVolume = 1.f;
	UAudioComponent* WindSoundComponent;

	float Interval = 1.f;
	float RemainingTime;

	FTimerHandle TickHandle;
	FTimerDelegate TickDelegate;

	AController* PlayerController;
	ACharacter* PlayerCharacter;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void HandleTick();
};
