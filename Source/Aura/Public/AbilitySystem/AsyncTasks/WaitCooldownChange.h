// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "WaitCooldownChange.generated.h"


struct FActiveGameplayEffectHandle;
struct FGameplayEffectSpec;
class UAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FCooldownChangeSignature, float, TimeRemaining);

/**
 * 等待冷却变化的异步任务
 * 监听冷却标签的变化，广播冷却开始和结束的事件
 * 通过监听ActiveGameplayEffectAddedDelegateToSelf来检测冷却效果的应用，以便在冷却开始时获取剩余时间
 * 当冷却标签被移除时，广播冷却结束事件
 * 通过EndTask函数来结束任务，移除监听
 */

UCLASS(BlueprintType, meta = (ExposedAsyncProxy = "AsyncTask"))
class AURA_API UWaitCooldownChange : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownStart;

	UPROPERTY(BlueprintAssignable)
	FCooldownChangeSignature CooldownEnd;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UWaitCooldownChange* WaitForCooldownChange(UAbilitySystemComponent* AbilitySystemComponent,const FGameplayTag& InCooldownTag);
	/*	静态工厂函数的参数  →  蓝图节点的输入引脚
		meta修饰符      →  控制引脚的显示和行为
		BlueprintAssignable	→ 白色输出节点
		TimeRemaining → 蓝色输出节点
	*/
	UFUNCTION(BlueprintCallable)
	void EndTask();

protected:
	
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> ASC;
	
	FGameplayTag CooldownTag;
	
	void CooldownTagChanged(const FGameplayTag InCooldownTag, int32 NewCount);
};
