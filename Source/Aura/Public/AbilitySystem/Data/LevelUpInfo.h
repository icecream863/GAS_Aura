// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"

/**
 * 
 */

USTRUCT(BlueprintType)
struct FAuraLevelUpInfo
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;// 升级需求的经验值
	
	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;// 属性点 奖励
	
	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1;// 技能点 奖励
	
};

UCLASS()
class AURA_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LevelUpInformation")
	TArray<FAuraLevelUpInfo> LevelUpInformation;
	
	int32 FindLevelForXP(int32 XP) const ;
};
