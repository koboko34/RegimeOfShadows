// Armand Yilinkou, 2023

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "LifeStealInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class ULifeStealInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class REGIMEOFSHADOWS_API ILifeStealInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	virtual void ApplyLifeSteal(float Damage) {}
};
