// Armand Yilinkou, 2023


#include "Decal.h"
#include "Components/DecalComponent.h" 

ADecal::ADecal()
{
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	Decal = CreateDefaultSubobject<UDecalComponent>("Decal");
	SetRootComponent(Decal);
}
