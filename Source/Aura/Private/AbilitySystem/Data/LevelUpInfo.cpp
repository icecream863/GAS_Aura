// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/LevelUpInfo.h"

int32 ULevelUpInfo::FindLevelForXP(int32 XP) const 
{
	
	int Level = 1; 
	bool bSearching = true;
	
	while (bSearching)
	{
		// 这个循环会一直检查当前等级的升级需求，直到找到一个等级，其升级需求大于当前经验值（XP）。
		// 当找到这样的等级时，循环停止，返回当前等级。
		// 下标为0只是占位，真正的等级从1开始，
		// 所以当 Level 达到 LevelUpInformation 数组的最后一个元素时，就停止循环，返回当前等级。
		if (Level >= LevelUpInformation.Num() - 1) return Level;
		
		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			Level++;
		}
		else
		{
			bSearching = false;
		}
	}
	
	return Level;
}
