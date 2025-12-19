// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearch = true;
	while (bSearch)
	{
		if (LevelUpInformation.Num() - 1 <= Level) return Level;

		if (XP >= LevelUpInformation[Level].LevelUpRequirement) Level++;
		else bSearch = false;
	}
	return Level;
}
