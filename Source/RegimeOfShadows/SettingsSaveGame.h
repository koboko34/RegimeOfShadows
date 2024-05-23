	// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "SettingsSaveGame.generated.h"

/**
 * 
 */
UCLASS()
class REGIMEOFSHADOWS_API USettingsSaveGame : public USaveGame
{
	GENERATED_BODY()
	
public:
	UPROPERTY(VisibleAnywhere)
	float MouseSensitivity = 1.f;
	UPROPERTY(VisibleAnywhere)
	float MasterVolume = 1.f;
	UPROPERTY(VisibleAnywhere)
	float MusicVolume = 1.f;

public:
	UFUNCTION(BlueprintPure)
	float GetMouseSensitivity() const { return MouseSensitivity; }
	UFUNCTION(BlueprintCallable)
	void SetMouseSensitivity(float NewSens);

	UFUNCTION(BlueprintPure)
	float GetMasterVolume() const { return MasterVolume; }
	UFUNCTION(BlueprintCallable)
	void SetMasterVolume(float NewVolume);

	UFUNCTION(BlueprintPure)
	float GetMusicVolume() const { return MusicVolume; }
	UFUNCTION(BlueprintCallable)
	void SetMusicVolume(float NewVolume);

};
