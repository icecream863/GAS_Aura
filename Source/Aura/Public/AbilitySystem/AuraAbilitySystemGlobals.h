// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "AuraAbilitySystemGlobals.generated.h"

/**
 * 项目自定义的 AbilitySystemGlobals。
 *
 * UE/GAS 的 UAbilitySystemGlobals 是一个“全局单例配置对象”，负责：
 * - 初始化/持有一些全局表（如 TargetData 序列化、GlobalCurveTable、TagResponseTable 等）
 * - 提供创建 GameplayEffectContext 的工厂函数（AllocGameplayEffectContext）
 *
 * 为什么要自定义？
 * - 默认的 FGameplayEffectContext 不包含我们自定义的字段（例如：是否格挡/是否暴击）
 * - 我们实现了 FAuraGameplayEffectContext（见 `AuraAbilityTypes.h`），并在其中实现 NetSerialize
 *   让这些字段能从服务端同步到客户端（用于 UI/飘字/GameplayCue 等）
 *
 * 工作方式：
 * - GAS 在创建 EffectSpec / EffectContext 时，会通过 UAbilitySystemGlobals::AllocGameplayEffectContext()
 *   来分配一个新的 Context。
 * - 我们在这里 override 该函数，返回 `FAuraGameplayEffectContext`，从而让整个项目默认都使用自定义 Context。
 *
 * 配置注意：
 * - 需要在 `Config/DefaultGame.ini` 里设置 AbilitySystemGlobalsClassName，指向该类，
 *   否则引擎仍会使用默认的 UAbilitySystemGlobals。
 */
UCLASS()
class AURA_API UAuraAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
public:
	/**
	 * 分配一个新的 GameplayEffectContext。
	 *
	 * 返回值所有权：
	 * - 调用方（GAS）会把它包装进 FGameplayEffectContextHandle 并负责生命周期管理。
	 * - 因此这里使用 `new` 是 GAS 的惯例写法。
	 */
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
