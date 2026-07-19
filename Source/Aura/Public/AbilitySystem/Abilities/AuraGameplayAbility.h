// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "Input")
	FGameplayTag StartupInputTag;
	
	/** 生成技能当前等级的富文本描述，具体技能可重写。 */
	virtual FString GetDescription(int32 Level);

	/** 生成技能下一等级的富文本预览，具体技能可重写。 */
	virtual FString GetNextLevelDescription(int32 Level);

	/** 生成技能尚未解锁时显示的等级要求。 */
	static FString GetLockedDescription(int32 Level);

protected:
	float GetManaCost(float InLevel = 1.f) const;
	float GetCooldown(float InLevel = 1.f) const;

};
