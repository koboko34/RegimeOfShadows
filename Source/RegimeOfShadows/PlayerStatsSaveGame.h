// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "PlayerStatsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class REGIMEOFSHADOWS_API UPlayerStatsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	int Level;
	UPROPERTY()
	int Experience;
	UPROPERTY()
	int ExpToNextLevel;

public:
	UFUNCTION(BlueprintPure)
	int GetLevel() const { return Level; }
	UFUNCTION(BlueprintPure)
	int GetExperience() const { return Experience; }
	UFUNCTION(BlueprintPure)
	int GetExpToNextLevel() const { return ExpToNextLevel; }
	UFUNCTION(BlueprintCallable)
	void SetLevel(int NewLevel);
	UFUNCTION(BlueprintCallable)
	void SetExperience(int NewExperience);
	UFUNCTION(BlueprintCallable)
	void SetExpToNextLevel(int NewExpToNextLevel);
};
