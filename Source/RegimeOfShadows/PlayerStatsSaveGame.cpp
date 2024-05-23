// Armand Yilinkou, 2023


#include "PlayerStatsSaveGame.h"

void UPlayerStatsSaveGame::SetLevel(int NewLevel)
{
	Level = NewLevel;
}

void UPlayerStatsSaveGame::SetExperience(int NewExperience)
{
	Experience = NewExperience;
}

void UPlayerStatsSaveGame::SetExpToNextLevel(int NewExpToNextLevel)
{
	ExpToNextLevel = NewExpToNextLevel;
}
