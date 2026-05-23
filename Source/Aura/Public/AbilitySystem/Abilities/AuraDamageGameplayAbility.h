// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"
#include "AuraDamageGameplayAbility.generated.h"


/**
 * 带伤害类型的技能，比如火焰伤害、冰霜伤害等。它的核心作用是：在执行技能时，
 * 能把伤害类型信息传递给 GameplayEffect，从而让 GameplayEffect 能根据伤害类型做出不同的效果
 * （比如火焰伤害会附带 DOT 效果，冰霜伤害会降低目标移动速度等）。还有抗性
 * 本质就是标签
 */

struct FTaggedMontage;

UCLASS()
class AURA_API UAuraDamageGameplayAbility : public UAuraGameplayAbility
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor); 
	
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	
	//伤害类型 及 对应的 数值
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypeMap; //set by caller，先set 再 get
	
	UFUNCTION(BlueprintPure)
	FTaggedMontage GetRandomTaggedMontageFromArray(const TArray<FTaggedMontage>& TaggedMontages);
};
